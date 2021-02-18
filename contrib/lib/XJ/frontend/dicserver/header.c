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
 *	Original Source: ./lib/Wnn/uum/header.c
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
#include "xwnmo.h"
#include "jllib.h"
#include "config.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "buffer.h"

int not_redraw;
w_char *input_buffer;
wchar *return_buf;	/* return you mojiretsu buffer */
struct wnn_buf *bun_data_;

int env_kankana_id;

int	kanakan_sticky = 0;	/* ADD KURI */

int	cur_bnst_  = 0;	/* current bunsetsu pointer */

int crow;
/*int disp_mode_length = 5;*/  /* モード表示に必要なcolumn数 */
char	Term_Name[128];

int rubout_code = 127;
int kk_on = 0;
int quote_code = -1;
int puote_flag = 0;

int (*main_table[TBL_CNT][TBL_SIZE])();
char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
w_char jishopath[64];
w_char hindopath[64];
w_char fuzokugopath[64];

struct buf *c_b;
short pty_c_flag;
short tty_c_flag;

int cursor_invisible_fun = 0;
int excellent_delete = 1;

int *bunsetsu;
int *bunsetsuend;
int *touroku_bnst;
int touroku_bnst_cnt;
/* int bunsetsucnt = 0; */
w_char *kill_buffer;
w_char *remember_buf;	/* ADD KURI */
int kill_buffer_offset = 0;

int touroku_comment = 0;	/* Comment for Words in Word_add */

char uumrc_name[PATHNAMELEN] = {'\0'};
char envrc_name_in_uumrc[PATHNAMELEN] = {'\0'};
char uumkey_name_in_uumrc[PATHNAMELEN] = {'\0'};
#ifndef	X11
char convkey_name_in_uumrc[PATHNAMELEN] = {'\0'};
#else	X11
char cvt_fun_file[PATHNAMELEN] = {'\0'};
char cvt_meta_file[PATHNAMELEN] = {'\0'};
char jfewrc_name[PATHNAMELEN] = {'\0'};
#endif	!X11
char rkfile_name_in_uumrc[PATHNAMELEN] = {'\0'};
#ifndef	X11
short defined_by_option = 0;
#endif	!X11

/*int conv_lines = 1;*/
int flow_control = 0;

char jserver_name_in_uumrc[PATHNAMELEN] = { '\0' };  /* V3.0 */
char username[PATHNAMELEN] = { '\0' };  /* V3.0 */
char user_dic_dir[PATHNAMELEN] = {'\0'}; /* V3.0 */

int remove_cs_from_termcap = 1;

int maxbunsetsu;
int max_ichiran_kosu;
int maxchg;
int maxlength;

/*
wchar kugiri_str[MAXKUGIRI] = {' ',0};
*/

WNN_DIC_INFO *dicinfo;
int dic_list_size;

char *prgname;	/* プログラム名 argv[0] */
