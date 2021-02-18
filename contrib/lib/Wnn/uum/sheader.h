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
/**************************
 * header of standard i/o 
 **************************/

#include "jslib.h"

extern int 	not_redraw; /* c_b->bufferを用いていない時(エラーメッセージを
表示しているなど)に、リドローがかかってもリドローしないためのフラグ */
extern int 	maxchg;  /*一度に変換できる文字数 */

extern int	maxlength; /* 画面の横幅を表す */
extern w_char   *input_buffer; /* 画面制御に使うバッファ */

extern struct wnn_buf *bun_data_;

extern struct wnn_env *env_kanakan;	/* ADD KURI */
extern struct wnn_env *env_kankana;	/* ADD KURI */
extern struct wnn_env *env_kanarom;	/* ADD KURI */

extern int kanakan_sticky;	/* ADD KURI */
extern int kankana_sticky;	/* ADD KURI */
extern int kanarom_sticky;	/* ADD KURI */

extern int 	cur_bnst_ ;		/* current bunsetsu pointer */

/*extern  int 	b_suu_;		*/	/* hold bunsetsu suu */
				/* Use jl_bun_suu(bun_data_) */

extern char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
extern int (*main_table[TBL_CNT][TBL_SIZE])();	/* holding commands */

/*  extern wchar *p_holder; */	/* points the end of data in buffer. */

extern wchar *knj_buffer; /* 辞書ユーティリティー使用時の漢字バッファ */


extern int crow;
	/* holding row where i/f uses to display kanji line */

extern wchar *return_buf;
	/* 確定した漢字かな混じり文を返すためのバッファ */


extern char	Term_Name[];

extern int rubout_code; /* rubout に使われるコードを保持する */
extern int kk_on; /* 仮名漢字変換可能モードか否かを示すフラグ */
extern int quote_code; 
extern int quote_flag;

extern int max_history;
extern 	wchar jishopath[];
extern 	wchar hindopath[];
extern wchar fuzokugopath[];


extern short tty_c_flag;
extern short pty_c_flag;

extern int cursor_invisible_fun; /* flag that cursor_invisible is in the termcap entry */
extern int send_ascii_char; 	/* flag that if send ascii characters when the buffer is empty */
extern int excellent_delete;
extern int (*keyin_when_henkan_off)();

extern char envrc_name_in_uumrc[];
extern char uumkey_name_in_uumrc[];
extern char convkey_name_in_uumrc[];
extern char rkfile_name_in_uumrc[];
extern char kankana_env_name_in_uumrc[];	/* ADD KURI */
extern char kanarom_env_name_in_uumrc[];	/* ADD KURI */
extern short defined_by_option;

extern int conv_lines;
extern int flow_control ;
extern int henkan_off_flag;	/* 立ち上げ時に変換をon/offにした状態にします */
extern int henkan_on_kuten;	/* 句点入力で変換する/しない */

extern char jserver_name_in_uumrc[]; /* V3.0 */
extern char kankana_jserver_name_in_uumrc[];
extern char kanarom_jserver_name_in_uumrc[];
extern char username[]; /* V3.0 */
extern char user_dic_dir[]; /* V3.0 */

extern int remove_cs_from_termcap;

extern int disp_mode_length; /* V3.0 Alternate for MHL */


extern int kanji_buf_size;
extern int maxbunsetsu;
extern int max_ichiran_kosu;

/*
#define MAXKUGIRI 32
extern wchar kugiri_str[];
*/

extern WNN_DIC_INFO *dicinfo;
extern int dic_list_size;

extern int touroku_comment;
