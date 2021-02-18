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
 *	Original Source: ./lib/Wnn/uum/sdefine.h
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
/*************************
 * define of standard i/o
 *************************/

#ifndef	MAXNAMLEN
#define	MAXNAMLEN	62
#define	MAXPATHLEN	255
#endif	

#define MAXCHG 80 /* 解析可能文字数 */
	/*漢字バッファ(次候補、単語検索など)の大きさ */
#define MAX_ICHIRAN_KOSU 36 /* 一覧表示字の表示個数 */
#define MAXWORDS	MAXJIKOUHO  /* 単語検索での検索語数 */
#define NBUN	2 /* n文節解析 */
#define JISHO_PRIO_DEFAULT 5 /*辞書プライオリティのデフォルト値 */

/* 評価関数の係数 */
#define	HINDOVAL 3 
#define	LENGTHVAL 100
#define	JIRITUGOVAL 1
#define FLAGVAL 200 /*今使ったよビットの係数*/
#define JISHOPRIOVAL 5

#define TBL_CNT 11		/* key_table no kazu */
/* Two tables are add. one is for selecting zenkouho's, and the other is jisho-ichiran. */
/* One more table is add, which is used in inspect */
#define TBL_SIZE 256

#define ESCAPE_CHAR(c)	(((c) < ' ') || ((c) == 127))
#define NORMAL_CHAR(c)	(((c) >= ' ') && ((c) < 127))
#define NOT_NORMAL_CHAR(c)	(((c) >= 128) && ((c) < 256))
#define ONEBYTE_CHAR(c)		(!((c) & ~0xff)) /* added by Nide */
/*
#define NISEMONO(c)		((c) & 0x80000000)
#define HONMONO(c)		(!NISEMONO(c))
#define KANJI_CHAR(c)		((c) >= 256 )
*/
#define KANJI_CHAR(c)		(((c) >= 256 ) && ((c) & 0xff00) != 0x8e00)

#define zenkaku(x)(KANJI_CHAR(x) || ESCAPE_CHAR(x))
#define hankaku(x) (!zenkaku(x))
#define ZENKAKU_HYOUJI(x)     zenkaku(x)
        /* CHANGE AFTERWARD TO BE ABLE TO TREAT hakaku katakana. */

#define max(a , b) (((a) > (b))?(a) : (b))
#define min(a , b) (((a) <= (b))?(a) : (b))

#define numeric(x)	(((x >= S_NUM)&&(x <= E_NUM))? True : NIL)
#define ISKUTENCODE(x)	((x) == 0xa1a3)

struct jisho_ { /* 辞書管理用structure */
    char name[1024];
    char hname[1024]; /* 頻度ファイル名 */
    int dict_no; /* server が返してくる辞書ナンバー */
    int prio;
    int rdonly;
};

struct kansuu{			/* kansuu_hyo no entry */
    char *kansuumei;
    char *comment;
    int romkan_flag;
    int  (*func[TBL_CNT])();
};

#ifndef wchar
#define	wchar	unsigned short
#endif

#ifdef	WNMO
/* Wnmo では、disp_mode_length は、固定長になっています
 * 可変長にするには、真面目に考える必要があります。
 * 取りあえず手抜き工事 */
/* #define MHL -1 */
#define MHL0 5  /* モード表示に必要なcolumn数 */
#ifndef	X11
extern int _disp_window;
#define disp_mode_length ((_disp_window == 0) ? 5 : -1) /* モード表示に必要なcolumn数 */
#else	X11
#define disp_mode_length -1 /* モード表示に必要なcolumn数 */
#endif	X11
#endif	WNMO
#define throw_c(col)  throw_col((col) + disp_mode_length)

#define MAX_HISTORY 10

#ifndef	WNMO
#define flush() fflush(stdout)
#else	WNMO
#define flush() ;
#endif	!WNMO
#ifndef	WNMO
#define print_msg(X) {push_cursor();throw_c(0); clr_line();printf(X);flush();pop_cursor();}
#define print_msg_getc(X) {push_cursor();throw_c(0); clr_line();printf(X);flush();romkan_next();pop_cursor();}
#endif	WNMO


#define J_IUJIS 0
#define J_EUJIS 1
#define J_JIS   2
#define J_SJIS  3

#define UNDER_LINE_MODE (0x02 | 0x08 | 0x20)

#define OPT_CONVKEY		0x01
#define OPT_RKFILE		0x02
#define OPT_WNNKEY		0x04
#define OPT_FLOW_CTRL		0x08
#define OPT_WAKING_UP_MODE	0x10
#define OPT_VERBOSE		0x20
#define OPT_DISP_AT_CURSOR	0x40
#ifdef	X11
#define OPT_FUNFILE		0x100
#define OPT_MTFILE		0x200
#define OPT_ASCFONT		0x400
#define OPT_KANJIFONT		0x800
#define OPT_KANAFONT		0x1000
#define OPT_GAIJIFONT		0x2000
#define OPT_UTILASCFONT		0x4000
#define OPT_UTILKANJIFONT	0x8000
#define OPT_UTILKANAFONT	0x10000
#define OPT_UTILGAIJIFONT	0x20000
#define OPT_FOREGROUND		0x40000
#define OPT_BACKGROUND		0x80000
#define OPT_BORDERGROUND	0x100000
#define OPT_DISPLAYNAME		0x200000
#endif	X11

#define convkey_defined_by_option	(defined_by_option & OPT_CONVKEY)
#define rkfile_defined_by_option	(defined_by_option & OPT_RKFILE)
#define uumkey_defined_by_option	(defined_by_option & OPT_WNNKEY)
#define verbose_option			(defined_by_option & OPT_VERBOSE)

#define char_len(X)((hankaku(X))? 1 : 2)

/*
  GETOPT string & ALL OPTIONS string for configuration.
  see each config.h for detail.
  NOTE: WHEN YOU MODIFY THESE, YOU ALSO MODIFY do_opt[] ARRAY AND
  ALL config.h!!!!
 */

#ifndef	WNMO
#define GETOPTSTR   "hHujsUJSPxXk:c:r:l:D:n:v"
#define ALL_OPTIONS "hHujsUJSPxXkcrlDnv"
#endif

#ifdef	X11
#define CVTFUNFILE	"/usr/local/lib/wnn/cvt_fun_tbl"
#define CVTMETAFILE	"/usr/local/lib/wnn/cvt_meta_tbl"

#define CANCEL		1
#define KANJI_IN_START	2
#define KANJI_IN_END	3
#define YOMI_IN		4
#define HINSI_IN	5
#define JISHO_IN	6
#define EXECUTE		7
#define TOUROKU_GO	8
/* 辞書操作用モード */
#define ICHIRAN		(1<<0)
#define TOUROKU		(1<<1)
#define JOKOUHOICHIRAN	(1<<2)
/* 一覧表用モード */
#define KENSAKU		20
#define NYUURYOKU	21
#define SENTAKU		22
#define JIKOUHO		23
#define KUTEN_IN	24
#define JIS_IN		25
#endif  X11
