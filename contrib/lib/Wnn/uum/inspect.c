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
#include "config.h"
#include "buffer.h"


inspect(bun_no)
int bun_no;
{
    char buf[1024];
    char *c = buf;

  if(make_string_for_ke(bun_no,buf,sizeof(buf)) == -1){
    print_msg_getc("Error in inspect");
    return(0);
  }
  select_line_element(&c,1,0,"",0,0,main_table[10]);
  return(1);
}

static int next_flag;
#define I_NEXT 1
#define I_PREV 2
#define I_CUR 3

inspect_kouho()
{
  push_cursor();
  next_flag = I_CUR;
  while(next_flag){
      if(next_flag == I_NEXT || next_flag == I_PREV){
	  if (jl_zenkouho_bun(bun_data_) != cur_bnst_ ||
	      jl_zenkouho_daip(bun_data_) != WNN_SHO){
	      jl_zenkouho(bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
	  }
	  if(next_flag == I_NEXT){
	      jl_next(bun_data_);
	  }else{
	      jl_previous(bun_data_);
	  }
	  make_kanji_buffer(cur_bnst_);
	  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)]; /* ADD KURI */
	  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1]; /* ADD KURI */
      }
      next_flag = 0;
      inspect(cur_bnst_);
  }
  pop_cursor();
  t_print_l();
  return(0);
}

/*
 *  Very Dirty, but we permit only this inspect function to manipulate
 *  directly the contents of bun_data_.
 */

static int Bun_no;

make_string_for_ke(bun_no,buf,buf_size)
int bun_no;
char *buf;
int buf_size;
{
    struct wnn_jl_bun *bun;
    wchar buf1[1024];
    wchar save_c;
    char tmp[WNN_HINSI_NAME_LEN * 2];
    
    Bun_no = bun_no;
    bun = bun_data_->bun[bun_no];
    jl_get_kanji(bun_data_, bun_no, bun_no + 1, buf1);
    sStrcpy(buf,buf1);
    strcat(buf," ");
    jl_get_yomi(bun_data_, bun_no, bun_no + 1, buf1);
/*
    bcopy(buf1 + bun->jirilen , buf1 + bun->jirilen + 1,
	  (1 + Strlen(buf1 + bun->jirilen)) * sizeof(wchar)); 
    buf1[bun->jirilen] = (wchar)'-';
    sStrcpy(buf + strlen(buf),buf1);
*/
    save_c = *(buf1 + bun->jirilen);
    *(buf1 + bun->jirilen) = 0;
    sStrcpy(buf + strlen(buf), buf1);
    *(buf1 + bun->jirilen) = save_c;
    strcat(buf, "-");
    sStrcpy(buf + strlen(buf), buf1 + bun->jirilen);

    set_escape_code(buf);	/* ADD KURI */

    strcat(buf,"   (");
    if(bun->dic_no >= 0){
	update_dic_list(bun_data_);
	dic_nickname(bun->dic_no, buf + strlen(buf));
	sStrcpy(tmp, jl_hinsi_name(bun_data_,bun->hinsi));
	sprintf(buf + strlen(buf), ":%d %s",
		bun->entry,tmp);
	if(bun->hindo != 0x7f){
	    sprintf(buf + strlen(buf), " 頻度:%c%d",
		    bun->ima ? '*' : ' ',
		    bun->hindo);
	}else{
	    sprintf(buf + strlen(buf), " ---------");
	}			/* Means This entry is not used.
				   This must not appear in ordinary use!*/

    }else{
	sStrcpy(tmp, jl_hinsi_name(bun_data_,bun->hinsi));
	sprintf(buf + strlen(buf),"GIJI BUNSETU:%s", tmp);
    }
    strcat(buf,")   ");
    sprintf(buf + strlen(buf),"評価値:%d 前端ベクトルNo.:%d",
	    bun->hyoka,bun->kangovect);
	    

    if(bun->dai_top){
	if(bun->daihyoka != -1){
	    sprintf(buf + strlen(buf),"(大文節評価値:%d) ",bun->daihyoka);
	}else {
	    sprintf(buf + strlen(buf),"(大文節評価値:---) ");
	}
    }

    return(0);
}

sakujo_kouho()
{
  push_cursor();
  sakujo_kouho1(cur_bnst_);
  pop_cursor();
  t_print_l();
  return(0);
}

sakujo_kouho1(bun_no)
int bun_no;
{
  w_char w_buf[512];
  char buf[512];
  int kanji_len = jl_kanji_len(bun_data_, bun_no, bun_no+1) - 
      jl_fuzoku_len(bun_data_, bun_no);

  jl_get_kanji(bun_data_, bun_no, bun_no + 1, w_buf);
  w_buf[kanji_len] = 0;
  w_sttost(w_buf, buf);
  strcat(buf,"  読み:");
  jl_get_yomi(bun_data_, bun_no, bun_no + 1, w_buf);
  w_buf[jl_jiri_len(bun_data_, bun_no)] = 0;
  w_sttost(w_buf,buf + strlen(buf));
  set_escape_code(buf);	/* ADD KURI */
  strcat(buf," ");
  sprintf(buf + strlen(buf), "を削除しますか?(Y/N)");
  if(yes_or_no(buf) == 1){
    if(jl_word_delete(bun_data_, bun_data_->bun[bun_no]->dic_no, 
		      bun_data_->bun[bun_no]->entry) == -1){
      errorkeyin();
      return;
    }
  }
}

inspectdel()
{
    struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
    int type;
    wchar buf1[1024];
    char buf[512];
    int kanji_len = jl_kanji_len(bun_data_, Bun_no, Bun_no+1) - 
	jl_fuzoku_len(bun_data_, Bun_no);

    type = dicinfo[find_dic_by_no(bun->dic_no)].type;
    if(type  != WNN_UD_DICT && type != WNN_REV_DICT){
	print_msg_getc("固定形式の辞書のエントリは削除出来ません。(如何?)");
	return(0);
    }
    if(dicinfo[find_dic_by_no(bun->dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc("リードオンリーの辞書のエントリは削除出来ません。(如何?)");
	return(0);
    }
	
    jl_get_kanji(bun_data_, Bun_no, Bun_no + 1, buf1);
    buf1[kanji_len] = 0;

    strcpy(buf, "「");
    w_sttost(buf1, buf + strlen(buf));
    sprintf(buf + strlen(buf), "」を削除します。(Y/N)");
    set_escape_code(buf);	/* ADD KURI */
    if(yes_or_no(buf) == 1){
	if(jl_word_delete(bun_data_, bun->dic_no, 
			  bun->entry) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

inspectuse()
{
    struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
    wchar buf1[1024];
    char buf[512];
    int kanji_len = jl_kanji_len(bun_data_, Bun_no, Bun_no+1) - 
	jl_fuzoku_len(bun_data_, Bun_no);

    if(dicinfo[find_dic_by_no(bun->dic_no)].hindo_rw == WNN_DIC_RDONLY){
	print_msg_getc("リードオンリーの頻度で変更で来ません。(如何?)");
	return(0);
    }
    strcpy(buf, "「");
    jl_get_kanji(bun_data_, Bun_no, Bun_no + 1, buf1);
    buf1[kanji_len] = 0;
    w_sttost(buf1, buf + strlen(buf));
    set_escape_code(buf);	/* ADD KURI */
    sprintf(buf + strlen(buf), "」の使用を中止します。(Y/N)");
    if(yes_or_no(buf) == 1){
	if(jl_word_use(bun_data_, bun->dic_no, bun->entry) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

next_inspect()
{
    next_flag = I_NEXT;
    return(1);
}

previous_inspect()
{
    next_flag = I_PREV;
    return(1);
}
