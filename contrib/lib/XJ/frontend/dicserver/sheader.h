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
 *	Original Source: ./lib/Wnn/uum/sheader.h
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
/**************************
 * header of standard i/o 
 **************************/

#include "jslib.h"

extern int	not_redraw; /* c_b->buffer���Ѥ��Ƥ��ʤ���(���顼��å�������
ɽ�����Ƥ���ʤ�)�ˡ���ɥ��������äƤ��ɥ����ʤ�����Υե饰 */
extern int	maxchg;  /*���٤��Ѵ��Ǥ���ʸ���� */

extern int	maxlength; /* ���̤β�����ɽ�� */
extern w_char	*input_buffer; /* ��������˻Ȥ��Хåե� */

extern struct wnn_buf *bun_data_;

extern int env_kankana_id;

extern int kanakan_sticky;
extern int	cur_bnst_ ;		/* current bunsetsu pointer */

/*extern int	b_suu_;		*/	/* hold bunsetsu suu */
				/* Use jl_bun_suu(bun_data_) */


extern char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
extern int (*main_table[TBL_CNT][TBL_SIZE])();	/* holding commands */

extern wchar *knj_buffer; /* ����桼�ƥ���ƥ������ѻ��δ����Хåե� */

extern int crow;
	/* holding row where i/f uses to display kanji line */

extern wchar *return_buf;
	/* ���ꤷ���������ʺ�����ʸ���֤�����ΥХåե� */

extern char	Term_Name[];

extern int rubout_code; /* rubout �˻Ȥ��륳���ɤ��ݻ����� */
extern int kk_on; /* ��̾�����Ѵ���ǽ�⡼�ɤ��ݤ��򼨤��ե饰 */
extern int quote_code; 
extern int qoute_flag; 

extern 	wchar jishopath[];
extern 	wchar hindopath[];
extern wchar fuzokugopath[];

extern int cursor_invisible_fun; /* flag that cursor_invisible is in the termcap entry */
extern int excellent_delete;
extern int (*keyin_when_henkan_off)();

extern char uumrc_name[];
extern char envrc_name_in_uumrc[];
extern char uumkey_name_in_uumrc[];
#ifndef	X11
extern char convkey_name_in_uumrc[];
#else	X11
extern char cvt_fun_file[];
extern char cvt_meta_file[];
extern char display_name[];
extern char jfewrc_name[];
#endif	!X11
extern char rkfile_name_in_uumrc[];
#ifndef	X11
extern short defined_by_option;
#endif	!X11

#define conv_lines 1
extern int flow_control ;
extern int henkan_off_flag;	/* Ω���夲�����Ѵ���on/off�ˤ������֤ˤ��ޤ� */
extern int henkan_on_kuten;	/* �������Ϥ��Ѵ�����/���ʤ� */

extern char jserver_name_in_uumrc[]; /* V3.0 */
extern char username[]; /* V3.0 */
extern char user_dic_dir[]; /* V3.0 */

extern int jserver_connected; /* V3.0 */

extern int remove_cs_from_termcap;

/*extern int disp_mode_length;*/ /* V3.0 Alternate for MHL */

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

extern char *prgname;

/*
#ifdef	X11
extern int      touroku_mode;
extern int      jutil_mode;
extern int      jutil_nyuu;
#endif	X11
*/
extern int      select_return;
