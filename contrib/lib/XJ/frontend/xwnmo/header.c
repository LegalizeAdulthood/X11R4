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

int rubout_code = 127;
int kk_on = 0;
int quote_code = -1;

char romkan_clear_tbl[TBL_CNT][TBL_SIZE];

w_char jishopath[64];
w_char hindopath[64];
w_char fuzokugopath[64];

int (*keyin_when_henkan_off)();

#ifndef	X11
char convkey_name_in_wnnrc[PATHNAMELEN] = {'\0'};
short defined_by_option = 0;
#else	X11
char cvt_fun_file[PATHNAMELEN] = {'\0'};
char cvt_meta_file[PATHNAMELEN] = {'\0'};
int defined_by_option = 0;
#endif	!X11

char jserver_name_in_uumrc[PATHNAMELEN] = { '\0' };  /* V3.0 */
char kankana_jserver_name_in_uumrc[PATHNAMELEN] = { '\0' };
char kanarom_jserver_name_in_uumrc[PATHNAMELEN] = { '\0' };
char user_dic_dir[PATHNAMELEN] = {'\0'}; /* V3.0 */

/*
wchar kugiri_str[MAXKUGIRI] = {' ',0};
*/

WNN_DIC_INFO *dicinfo;
int dic_list_size;

char *prgname;	/* プログラム名 argv[0] */
#ifdef	X11
char fore_color[MAXNAMLEN] = { 0 };  /* フォアグランド色名   */
char back_color[MAXNAMLEN] = { 0 };  /* バックグランド色名   */
char boder_color[MAXNAMLEN] = { 0 };/* ボーダーグランド色名  */

char xwnmo_title[80] = { 0 };        /* タイトル表示文字列 */
char ascii_font[MAXFONTNMLEN] = { 0 };/* 半角フォント名 */
char kanji_font[MAXFONTNMLEN] = { 0 };/* 全角フォント名 */
char kana_font[MAXFONTNMLEN] = { 0 };/* kanaフォント名 */
char gaiji_font[MAXFONTNMLEN] = { 0 };/* 外字フォント名 */
char util_ascii_font[MAXFONTNMLEN] = { 0 };/* 半角フォント名 */
char util_kanji_font[MAXFONTNMLEN] = { 0 };/* 全角フォント名 */
char util_kana_font[MAXFONTNMLEN] = { 0 };/* kanaフォント名 */
char util_gaiji_font[MAXFONTNMLEN] = { 0 };/* 外字フォント名 */
char ascii_escape[4] = { 0 };/* 半角フォント名 */
char kanji_escape[4] = { 0 };/* 全角フォント名 */
char kana_escape[4] = { 0 };/* kanaフォント名 */
char gaiji_escape[4] = { 0 };/* 外字フォント名 */

char display_name[MAXNAMLEN] = { 0 };/* ホストマシン名       */

int def_width = 0;          /* xwnmoウィンドウデフォルト幅 */
int def_bwidth = 0;         /* xwnmoウィンドウデフォルトボーダー幅 */
int jumpkey_mode = 0;       /* 不必要なキーをクライアントに返すかどうか
				0 : 返さない(捨てる)
				1 : 返す			*/
char uumrc_name_in_jfewrc[MAXNAMLEN] = { 0 };
struct User_uumrc user_uumrc[MAX_CLIENTS];

#endif	X11
