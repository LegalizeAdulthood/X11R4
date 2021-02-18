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
 *	Original Source: ./lib/Wnn/uum/key_bind.c
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
    0: 変換後、変換結果を修正しているモード
    1: 変換前の文字列を入力しているモード
    2: 変換後、文節の区切りを伸ばしたり縮めたりしているモード
    3: バッファに文字が何も入っていないモード
    4: カーソルを動かしながら、何か候補を選択する時のモード
    5: 単語登録時に、漢字の範囲を設定している時のモード
    6: 単語登録時の読み入力などの文字列を入力している時
    7: 単語検索や、単語削除、辞書検索、辞書削除などのモード
    8: inspect, 単語検索など、単語の情報を出している時のモード
    9: 辞書検索のモード
    10:インスペクトのモード
*/    


#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "sheader.h"
#define MADA NULL

extern insert_it_as_yomi();
extern int forward();
extern int backward();
extern int c_top();
extern int c_end();
extern int t_quit();
extern int t_delete_char();
extern int delete_c();
extern int t_rubout();
extern int rubout_c();
extern int t_markset();
extern int t_kill();
extern int kill_c();
extern int t_next();
extern int t_previous();
extern int t_jmp_forward();
extern int t_jmp_backward();
extern int t_ret();
extern int input_from_kk();
extern int backward_c();
extern int quote();
extern int reconnect_jserver();

extern int select_select();
extern int quit_select();

extern int kdicdel();
extern int kdicuse();
extern int kdiccom();
extern int kworddel();
extern int kworduse();
extern int kwordcom();

static struct kansuu kansuu_hyo[] = {
{ "send_string" , "send what is in the buffer to pty." ,0,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "forward_char" ,"cursor wo hitotu maeni susumeru." , 1, NULL ,forward ,NULL , NULL , NULL,forward ,forward , NULL, NULL, NULL, NULL},
{ "backward_char" ,"cursor wo hitotu usironi susumeru.", 1,NULL ,backward_c ,NULL ,NULL,NULL,backward,backward,NULL, NULL, NULL, NULL},
{ "goto_top_of_line" ,"",1  ,NULL ,c_top ,NULL ,NULL ,NULL ,c_top, c_top,NULL, NULL, NULL, NULL},
{ "goto_end_of_line" ,"",1  ,NULL, c_end ,c_end ,NULL ,NULL ,c_end, c_end,NULL, NULL, NULL, NULL},
/*{ "rubout","",NULL, rubout_c, NULL, NULL, NULL, t_rubout,t_rubout, NULL, NULL, NULL}, */
{ "delete_char_at_cursor" ,"",1 , NULL , delete_c,NULL , NULL , NULL , t_delete_char, t_delete_char, NULL, NULL, NULL, NULL},
{ "kaijo" ,"",0 , NULL , NULL , NULL , NULL , NULL  , NULL, NULL, NULL, NULL, NULL, NULL} ,
{ "henkan" ,"" ,1,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "tan_henkan" ,"" ,1,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "tan_henkan_dai" ,"" ,1,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "jikouho" ,"" ,0,  NULL,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "zenkouho" ,"" ,0,  NULL,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "jikouho_dai" ,"" ,0,  NULL,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "zenkouho_dai" ,"" ,0,  NULL,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "select_jikouho" ,"" ,0,  NULL, NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "select_jikouho_dai" ,"" ,0,  NULL, NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "kill" ,"" ,1, NULL ,  kill_c ,NULL , NULL , NULL , t_kill, t_kill, NULL, NULL, NULL, NULL},
{ "yank" ,"" ,1, NULL, NULL ,NULL ,NULL ,NULL ,NULL, NULL,NULL, NULL, NULL, NULL},
{ "yank_e" ,"" ,1, NULL, NULL ,NULL ,NULL ,NULL ,NULL, NULL,NULL, NULL, NULL, NULL},
{ "bunsetu_nobasi" ,"" ,0,  NULL, NULL , NULL, NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "bunsetu_chijime" ,"" ,0,  NULL, NULL , NULL, NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "jisho_utility" ,"" ,0,  NULL,NULL, NULL ,NULL, NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "jisho_utility_e" ,"" ,0,  NULL,NULL, NULL ,NULL, NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "touroku" , "",1, NULL,NULL,NULL,NULL,NULL, NULL, NULL,NULL, NULL, NULL, NULL},
{ "touroku_e" , "",1, NULL,NULL,NULL,NULL,NULL, NULL, NULL,NULL, NULL, NULL, NULL},
{ "kakutei" ,"" ,1,  NULL, NULL, NULL , NULL , NULL ,NULL,NULL, NULL, NULL, NULL, NULL},
{ "sainyuuryoku" ,"" ,1,  NULL , NULL, NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "sainyuuryoku_e" ,"" ,1,  NULL , NULL, NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "kuten" ,"" ,0,  NULL , NULL , NULL , NULL ,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "kuten_e" ,"" ,0,  NULL , NULL , NULL , NULL ,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "jis" ,"" ,0,  NULL , NULL, NULL , NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "jis_e" ,"" ,0,  NULL , NULL, NULL , NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "redraw_line" ,"" ,1,  NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "redraw_line_e" ,"" ,1,  NULL , NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "previous_history_e" ,"",1,  NULL, NULL, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "previous_history" ,"",1,  NULL, NULL, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "next_history_e" ,"",1,  NULL, NULL, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "next_history" ,"",1,  NULL, NULL, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "touroku_mark_set" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , t_markset,NULL, NULL, NULL, NULL, NULL},
{ "touroku_return" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , NULL, t_ret, NULL, NULL, NULL, NULL},
{ "quit" ,"" ,0,  NULL , NULL , NULL , NULL , quit_select , t_quit, t_quit, NULL, NULL, NULL, NULL},
{ "touroku_jump_forward" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , t_jmp_forward,NULL, NULL, NULL, NULL, NULL},
{ "touroku_jump_backward" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , t_jmp_backward,NULL, NULL, NULL, NULL, NULL},
{"change_to_insert_mode", "",0, insert_it_as_yomi,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "quote_e", "",0,  NULL, quote, NULL, quote, NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "quote", "",0,  NULL, quote, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "send_ascii_char","",0, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "toggle_send_ascii_char","",0, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "not_send_ascii_char","",0, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "pop_send_ascii_char","",0, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "quote_send_ascii_char","",0, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL}, /* ADD KURI END */
{ "select_select", "",0,  NULL, NULL, NULL, NULL, select_select, NULL, NULL, NULL, NULL, NULL, NULL},
{ "lineend_select", "",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL},
{ "linestart_select", "",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "previous_select", "",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "next_select", "",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "backward_select", "",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "forward_select", "",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "reconnect_jserver", "",0,  reconnect_jserver, reconnect_jserver, reconnect_jserver, reconnect_jserver, reconnect_jserver,reconnect_jserver,reconnect_jserver, reconnect_jserver,  reconnect_jserver,  reconnect_jserver,  reconnect_jserver},
{ "henkan_on", "",0,  NULL, NULL, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "inspect","",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "sakujo_kouho","",0,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "nobi_henkan_dai" ,"" ,0,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "nobi_henkan" ,"" ,0,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "del_entry","",0, NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, kworddel, kdicdel, NULL},
{ "use_entry","",0,  NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, kworduse, kdicuse, NULL},
{ "com_entry","",0,  NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, kwordcom, kdiccom, NULL},
{ "kana_henkan" ,"" ,1,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL}, /* ADD KURI for kankann */
{ "romaji_henkan" ,"" ,1,  NULL ,NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL}, /* ADD KURI for kankann */
{ "romaji_nobi_henkan" ,"" ,1,  NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};


int kansuu_hyo_entry_suu = sizeof(kansuu_hyo) / sizeof(struct kansuu);

static char key_binded[sizeof(kansuu_hyo)/sizeof(struct kansuu) + 1];

init_key_table()
{
 FILE *fp , *fopen(); char file_name[PATHNAMELEN];
 int j,k;

 for(k = 0 ; k < sizeof(kansuu_hyo)/sizeof(struct kansuu);k++){
   key_binded[k] = 0;
 }

   /* uumrcファイルから。*/
 if (uumkey_name_in_uumrc[0] != '\0') {
   strcpy(file_name, uumkey_name_in_uumrc);
 }else if(strcpy(file_name , CPFILE),(fp = fopen(file_name , "r")) != NULL){
   /* default */
   close(fp);
 }else{
/*	fprintf(stderr, "Can't open default uumkey.%s\n" , CPFILE); */
   fprintf(stderr,"デフォルトのuumkeyが異常です。 %s\n",CPFILE);
   return(-1);
 }

  for(j = 0 ; j < 3 ; j++){
/*
    for(k = 0; k < 32;k++){
      main_table[j][k] = null_fun;
    }
*/
    for(k = 0;k < TBL_SIZE;k++){
      main_table[j][k] = NULL;
    }
    main_table[j][NEWLINE] = NULL;
    main_table[j][CR] = NULL;
  }
  for(j = 3 ; j <TBL_CNT ; j++){
    for(k = 0;k < TBL_SIZE;k++){
      main_table[j][k] = NULL;
    }
  }
 if (key_binding(file_name) == -1)
   return(-1);
 return(0);
}


key_binding(file_name)
char file_name[64];
{                
  register int j,k,kk;
  FILE *fp , *fopen();
  char func_name[64];
  int com_cnt;
  int com_num ;
  char buf[256];
  char com_name[16][EXPAND_PATH_LENGTH];
  int lc = 0;
  int tbl;
  
  if((fp = fopen(file_name , "r")) == NULL){
    printf("Key_binding file \"%s\" doesn't exist.\r\n",file_name);
    return(-1);
  }
  
  /* KURI
  romkan_clear_tbl[rubout_code] = 0;
  */
  for(tbl = 0 ; tbl < TBL_CNT ; tbl++){ /* ADD KURI */
      romkan_clear_tbl[tbl][rubout_code] = 0; /* Not clear romkan when rubout */
  }
  main_table[1][rubout_code] = rubout_c;
  main_table[5][rubout_code] = t_rubout;
  main_table[6][rubout_code] = t_rubout;

  lc = 0;
  while(fgets(buf , 256 , fp)){
    lc += 1;
    if((*buf == ';') || (*buf == ':') || (*buf == '\n'))continue; /* for comment lines */
    if((com_cnt = sscanf(buf , "%s %s %s %s %s %s %s %s %s %s %s" , func_name , com_name[0],
	       com_name[1],com_name[2],com_name[3],com_name[4],com_name[5],com_name[6],com_name[7],
               com_name[8],com_name[9])) < 2){ /* able to get at most 9 arguments. */
      printf("uumkey:file %s. Line %d is illegal.\r\n" ,file_name, lc);
      continue;       /* every command needs at least 1 argument. */
    }

    if (strcmp(func_name, "include")== 0){
      if(0 != expand_expr(com_name[0])){
      	fprintf(stderr,"uumkey: %s を展開できません。\r\n", com_name[0]);
      }
      key_binding(com_name[0]);
    }else if (strcmp(func_name, "unset")== 0){
      if((k = find_entry(com_name[0])) != -1){
	key_binded[k] = 0;
	remove_key_bind(k);
      }else if(strcmp(com_name[0],"quote_keyin") == 0){
	quote_code = -1;
      }
    }else if (strcmp(func_name, "quote_keyin")== 0){
      if ((com_num = expand_argument(com_name[0])) < 0) {
	fprintf(stderr,"uumkey: %s is an inappropriate code for %s.\r\n",com_name[0],func_name);
	fflush(stderr);
      } else quote_code = com_num;
    } else {
      for(k = 0 ; k < kansuu_hyo_entry_suu ; k++){
	if (strcmp(func_name, kansuu_hyo[k].kansuumei)== 0){ /* found ! */
	  if(key_binded[k]){
	    remove_key_bind(k);
	  }
	  key_binded[k] = 1;
	  for(kk = 0 ; kk < com_cnt - 1 ; kk++) {
	    com_num = expand_argument(com_name[kk]);
	    if (com_num < 0) {
	      fprintf(stderr,"uumkey: %s is an inappropriate code for %s.\r\n",com_name[kk],func_name);
	      fflush(stderr);
	    } else {
	      /* change KURI
	      romkan_clear_tbl[com_num] = kansuu_hyo[k].romkan_flag;
	      */
	      for(tbl = 0 ; tbl < TBL_CNT ; tbl++){
		if ((main_table[tbl][com_num] != NULL) && 
/*		    (main_table[tbl][com_num] != null_fun) &&  */
		    (kansuu_hyo[k].func[tbl] != NULL)) {
		  /* fprintf(stderr, "uumkey: Duplicate Definition at key: %s.\n",com_name[kk]);*/
		  fprintf(stderr, "uumkey: %s(0x%x)のキーが二重定義です。at line %d (%s)(table No. %d).\r\n",
			  com_name[kk],com_num,lc,kansuu_hyo[k].kansuumei,tbl);
/*		  fprintf(stderr, "lc = %d,kansuu_hyo %d(%s) ,%d , \
                     %s(0x%x)のキーが二重定義です。\n",lc,k,kansuu_hyo[k].kansuumei,tbl ,com_name[kk],
			  com_num);
*/
		  main_table[tbl][com_num] = kansuu_hyo[k].func[tbl];
		  romkan_clear_tbl[tbl][com_num] = kansuu_hyo[k].romkan_flag; /*KURI*/
		} else if (kansuu_hyo[k].func[tbl] != NULL){
		  main_table[tbl][com_num] = kansuu_hyo[k].func[tbl];
		  romkan_clear_tbl[tbl][com_num] = kansuu_hyo[k].romkan_flag; /*KURI*/
		}
	      }
	    }
	  }
	  break;
	} /* end of routine when command is founded. */
       
	/* loop while command is founded */
      }
      if(k == kansuu_hyo_entry_suu){
	fprintf(stderr, "uumkey: unknown entry name %s.\r\n",func_name);
      }
    }
  }
  fclose(fp);
  return(0);
}

/** argument expansion. */
expand_argument(st)
char *st;
{
  int num;

  if(*st == '^'){
    num = (*(st + 1)) & 0x1f;
  } else if (*st == '0' || *st == '\\') {
    if (*(st + 1) == 'x' || *(st + 1) == 'X') {
      sscanf(st + 2,"%x",&num);
    } else {
      sscanf(st,"%o",&num);
    }
  } else if(!('0' <= *st && *st <= '9')){
    return(-1);
  } else {
    num = atoi(st);
  }
  if(num >= TBL_SIZE){
    printf("uumkey: You can only bind a function to code less than %d.\r\n",
    	   TBL_SIZE);
    return(-1);
  }
  return(num);
}

remove_key_bind(key)
int key;
{
  int tbl;
  int com;

  for(com = 0 ; com < TBL_SIZE; com++){
    for(tbl = 0 ; tbl < TBL_CNT ; tbl++){
      if (kansuu_hyo[key].func[tbl] != NULL) {
	if(main_table[tbl][com] == kansuu_hyo[key].func[tbl]){
	  main_table[tbl][com] = NULL;
	}
      }
    }
  }
}


find_entry(com_name)
char *com_name;
{
  int k;

  for(k = 0 ; k < kansuu_hyo_entry_suu ; k++){
    if (strcmp(com_name, kansuu_hyo[k].kansuumei)== 0){
      if(key_binded[k]){
	return(k);
      }
    }
  }
  return(-1);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
