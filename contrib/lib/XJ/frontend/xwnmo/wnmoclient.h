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
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	Version 4.0
 */
/*
	client.h
*/

#ifndef	JLLIB
#include "jllib.h"
#endif	JLLIB

#define FILENAME 128

#define MAX_CLIENTS 32

/* constants for one client */
#define USER_NAME_LEN 20
#define HOST_NAME_LEN 20

#define	romkan_getc	rk_getc
#define romkan_next	conv_keyin

#include "rk_header.h"
#include "rk_mult.h"


/* bufferの表示のされ方は、hantenによって決定される。
反転するか、下線状態にするかを各独立に、
バッファーの内で、文頭からmarkまで、markからcursorまで、
cursorからbufferの最後までの状態を独立に設定することができる。
	 when mark is set
        	hanten bit0,bit1:      --- mark
		bit2,bit3:        mark --- cursor
		bit4,bit5:      cursor --- 
	  when mark is not set(-1)
		bit4,bit5:      
    first one....hanten
    second one...under_line
*/
struct buf{
  wchar  *buffer;	/* 登録用バッファ */
  char *buf_msg; /** 仮名を入力する時のバッファーの前に表示する文字列 */
  int start_col; /** 仮名入力時の画面上のコラムのスタート位置 */
  int t_c_p; /** 仮名入力用バッファー上でのカーソル位置 */
  int hanten; /** 仮名入力用バッファーを反転表示させているかどうか */
  int maxlen; /**仮名入力用バッファーの中にはいっている文字数 */
  int buflen; /** length of the buffer */
  int t_m_start;  	/* -1 : markが設定されていない。*/
			/* 0 以上 : マークの位置*/
  int t_b_st;	/**ボールド表示の開始位置 */ /* add by KUWA */
  int t_b_end;	/**ボールド表示の終了位置 */ /* add by KUWA */
  int vlen;    /*バッファー中でスクリーンに表示される幅*/
  int duplicate;/* スクリーンがスクロールする時に、重複して表示する文字数*/

  int vst;    /* bufferの中で画面に表示されている最初の位置*/
		/*これは、勝手に上のルーチンでいじらない方が望ましい。*/
  int (*key_in_fun)(); /*ここに関数が設定されていると、key_tableにバインドが
  ない文字の入力を
  受けた時にバッファーにその文字を入れずに代わりにその関数が呼ばれる*/
  int (*redraw_fun)();/* romkan kara redraw ga kaette kitatoki */
  int (*ctrl_code_fun)();/*ここに関数が設定されていると、key_tableにバインドが
  ないコントロール文字の入力を受けた時にベルを鳴らす代わりに
  その関数が呼ばれる*/
  int (**key_table)();	/*キーバインドのテーブル*/
  char *rk_clear_tbl;	/* romakn_clear が hituyouka */
};

typedef	struct _main_table {
	int	(*m_table[11][256])();
} MAIN_TABLE;

/*
	Client
*/
#ifdef	GMW
#define	C_ID	o_Fragment	
#endif
#ifdef	X11
#define	C_ID	Window
#endif

#ifndef	X11
#define STACKLENGTH 20
struct cursor_state {
	int	colum;
	int	reverse;
	int	underline;
	int	invisible;
	int	bold;	/* add by KUWA */
};
#endif

struct wnmo_client {
	C_ID	c_id;		/* client id */
#ifdef	GMW
	struct	ttymodoki	*window;
#endif
#ifdef	X11
	/* Xwindow have this struct in client_mem[] */
#endif
	char	user_name[USER_NAME_LEN]; /* env_name */
	char	host_name[HOST_NAME_LEN]; /* not use now */

	int	ref_cnt;	/* reference count *//* not use now */

	int	two_byte_first;	/* not use now */
	wchar	wc;		/* not use now */
/**/

/* buffers */
	struct  wnn_buf *b_data_;	/* this means bun_data_ */
	struct  buf     *c_buf;		/* this means c_b */

	struct wnn_env  *e_kanakan;	/* kanakan_env */
	struct wnn_env  *e_kankana;	/* kanakan_env */
	struct wnn_env  *e_kanarom;	/* kanakan_env */

	int	kanakan_st;		/* ADD KURI */
	int	kankana_st;		/* ADD KURI */
	int	kanarom_st;		/* ADD KURI */

	char envrc_name[256];
	char kankana_env_name[256];
	char kanarom_env_name[256];
	char uumkey_name[256];
	char rkfile_name[256];
	int	*b_setsu;
	int	*b_setsuend;
	struct wnn_env **b_setsuenv;
	int	*t_bnst;
	int	t_bnst_cnt;

	wchar	*k_buffer;
	int	k_buffer_offset;
	wchar	*rem_buf;
	wchar   *i_buffer;	/* 画面制御に使うバッファ */
	wchar	*ret_buf;	/* 確定した漢字かな混じり文を返す */

	int	t_comment;	/* Comment for Words in Word_add */
/* key_table */
	MAIN_TABLE	*m_table;
/**/
	int	c_bnst_;	/* current bunsetsu */
	int	m_length;	/* 画面の横幅を表す */
	int	m_chg;		/* add by kuwari 1/3/7 */
	int	m_history;
	int	m_bunsetsu;
	int	m_ichiran_kosu;

/* cursol attribute */
	int	c_colum;		/* */
	int	c_reverse;
	int	c_underline;
	int	c_bold;		/* add by KUWA */
	int	c_invisible;
#ifndef	X11
	int	c_stack_top;
	struct	cursor_state	s_upscreen[STACKLENGTH];
#endif

/* internal flags */
	/*int	j_connected;*/	/* jserver to connect siteruka? */
	int	h_mode;
	int 	n_redraw;	/* c_b->bufferを用いていない時
				 (エラーメッセージを表示など)にリドローしない */
	int	q_flag;
	/*
	int	step;
	*/
	int	(*command_func)();	/* buufer_in から呼ばれる関数 */
	int	buffer_in;
	int	buffer_in_nest;
	int	buffer_ignored[4];

/* external flags */
	int	s_ascii_char;	 /* flag that if send ascii characters
				    when the buffer is empty */
	int	e_delete;
	int	h_flag;		/* 立ち上げ時に変換をon/offにします */
	int	h_on_kuten;	/* 句点入力で変換する/しない */
	int	h_off_def;
/* romkan */
	Romkan	*rk;
};

#define	BUFFER_IN_CONT	-2

struct wnmo_client *c_c;

extern struct wnmo_client *clients[];

/*  buffers */
#define	bun_data_		(c_c->b_data_)
#define	c_b			(c_c->c_buf)

#define	env_kanakan		(c_c->e_kanakan)
#define	env_kankana		(c_c->e_kankana)
#define	env_kanarom		(c_c->e_kanarom)

#define	kanakan_sticky		(c_c->kanakan_st)
#define	kankana_sticky		(c_c->kankana_st)
#define	kanarom_sticky		(c_c->kanarom_st)

#define envrc_name_in_uumrc	(c_c->envrc_name)
#define kankana_env_name_in_uumrc	(c_c->kankana_env_name)
#define kanarom_env_name_in_uumrc	(c_c->kanarom_env_name)
#define rkfile_name_in_uumrc	(c_c->rkfile_name)
#define uumkey_name_in_uumrc	(c_c->uumkey_name)

#define bunsetsu		(c_c->b_setsu)
#define bunsetsuend		(c_c->b_setsuend)
#define bunsetsu_env		(c_c->b_setsuenv)
#define touroku_bnst		(c_c->t_bnst)
#define touroku_bnst_cnt	(c_c->t_bnst_cnt)

#define kill_buffer		(c_c->k_buffer)
#define kill_buffer_offset	(c_c->k_buffer_offset)
#define remember_buf		(c_c->rem_buf)

#define	input_buffer		(c_c->i_buffer)	/* 画面制御に使うバッファ */
#define	return_buf		(c_c->ret_buf)

#define	touroku_comment		(c_c->t_comment)

#define	main_table		(c_c->m_table->m_table)
#define username		(c_c->user_name)

#define	cur_bnst_		(c_c->c_bnst_)	/* current bunsetsu */
#define	maxlength		(c_c->m_length)	/* 画面の横幅を表す */
#define	maxchg			(c_c->m_chg)	/* 画面の横幅を表す */
#define max_history		(c_c->m_history)
#define maxbunsetsu		(c_c->m_bunsetsu)
#define max_ichiran_kosu	(c_c->m_ichiran_kosu)

/* cursol attribute */
#define	cursor_colum		(c_c->c_colum)
#define	cursor_reverse		(c_c->c_reverse)
#define	cursor_underline	(c_c->c_underline)
#define	cursor_bold		(c_c->c_bold)	/* add by KUWA */
#define	cursor_invisible	(c_c->c_invisible)
#define	cursor_stack_top	(c_c->c_stack_top)
#define	savedupscreen		(c_c->s_upscreen)

/* internal falgs */
/*
#define	jserver_connected	(c_c->j_connected)
*/
#define	henkan_mode		(c_c->h_mode)
#define not_redraw		(c_c->n_redraw)
#define quote_flag		(c_c->q_flag)
#define ignored			(c_c->buffer_ignored[c_c->buffer_in_nest])

/* external modes */
#define	send_ascii_char		(c_c->s_ascii_char)
#define	excellent_delete	(c_c->e_delete)
#define	henkan_off_flag		(c_c->h_flag)
#define	henkan_on_kuten		(c_c->h_on_kuten)
#define	henkan_off_def		(c_c->h_off_def)


#ifdef	nodef
extern int (*main_table[TBL_CNT][256])();	/* holding commands */

extern int kk_on; /* 仮名漢字変換可能モードか否かを示すフラグ */
extern int quote_flag;

extern char uumkey_name_in_uumrc[];
extern char rkfile_name_in_uumrc[];
extern short defined_by_option;

extern int maxbunsetsu;
extern int max_ichiran_kosu;

extern struct uum_ret_buf dic_list;
extern int dic_list_size;
#endif
