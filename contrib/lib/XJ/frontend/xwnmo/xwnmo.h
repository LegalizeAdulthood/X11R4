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
/*********

	Ｘｗｉｎｄｏｗ対応ＷＮＭＯ　ヘッダ

*********/

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include "XJplib.h"
#include "wnmoclient.h"

#define XWNMO_GETOPTSTR	"hHk:r:D:n:vF:M:"
#define XWNMO_OPTIONS	"hHkrDnvFM"
#define WNMO_FRAGMENT_NAME      "WNMOFRAG"

#define MAX_CLIENT	32	/* 接続できるクライアントの最大数 */
#define MAX_SELECT	32	/* 接続できるクライアントの最大数
				   (ソケットインタフェース用)     */
#define MAXFONTNMLEN	256	/* フォント名文字列	*/
#define IN_BORDER	2

typedef	struct	_XJClient {
	long	dispmode;	/*  かな漢字変換ウィンドウの形態	*/
				/*  0 : 各ウィンドウの下部
				    1 : 各カーソル位置
				    2 : スクリーン下部(デフォルト)	*/
	unsigned char a_data[128];	/*  ユーザ名 (V3.1 までは未使用)*/
	Window	w;		/*  かな漢字変換要求ウィンドウＩＤ	*/
	unsigned long	mask;	/*  書く属性のマスク値			*/
	long	p_width;	/*  かな漢字変換要求ウィンドウの大きさ
							( ピクセル値 )	*/
	long	p_height;	/*  かな漢字変換要求ウィンドウの大きさ
							( ピクセル値 )	*/
	long	x;		/*  かな漢字変換ウィンドウの位置( ピクセル値 )*/
	long	y;		/*  かな漢字変換ウィンドウの位置( ピクセル値 )*/
	long	width;		/*  かな漢字変換ウィンドウの幅( ピクセル値 )  */
	long	height;		/*  かな漢字変換ウィンドウの高さ( ピクセル値 )*/
	unsigned long	fg;	/*  かな漢字変換ウィンドウのフォアグランド色  */
	unsigned long	bg;	/*  かな漢字変換ウィンドウのバックグランド色  */
	long	bp;		/*  変換ウィンドウの境界幅( ピクセル単位 )*/
	long	status_x;
	long	status_y;
	char	h_font[256];	/* かな漢字変換ウィンドウG0フォント名*/
	char	h_escape[4];
	char	k_font[256];	/* かな漢字変換ウィンドウG1フォント名*/
	char	k_escape[4];
	char	r_font[256];	/* かな漢字変換ウィンドウG2フォント名*/
	char	r_escape[4];
	char	g_font[256];	/* かな漢字変換ウィンドウG3フォント名	*/
	char	g_escape[4];
	Window		wp1;	/*  ダミーの親ウィンドウ		*/
	Window		wp2;	/*  このウィンドウの上を子ウィンドウ(w1,w2,w3)*/
	Window		wp3;    /*  が動いて、文字を表示する。		*/
	Window		w1;	/*  実際に文字が書かれるウィンドウ(１行目)*/
	Window		w2;	/*  実際に文字が書かれるウィンドウ(２行目)*/
	Window		w3;	/*  実際に文字が書かれるウィンドウ(３行目)*/
	Window		wnf;	/*  前がチョン切れている印用ウィンドウ	*/
	Window		wnb;	/*  後がチョン切れている印用ウィンドウ  */
	Window		wnn;	/*  半分チョン切れてしまった文字を消すための
				    ウィンドウ				*/
	Window		wp;
	Window		wt;
	Window		ws;
	long		status_lock;
	long		columns;/*  変換ウィンドウの全カラム数
				    (デフォルトのウィンドウしか使っていないよ)*/
	long		max_columns;/*  表示できる最大カラム数	*/
	XFontStruct	*h_f;	/*  G0フォントの構造体	*/
	XFontStruct	*k_f;	/*  G1フォントの構造体	*/
	XFontStruct	*r_f;	/*  G2フォントの構造体	*/
	XFontStruct	*g_f;	/*  G3フォントの構造体	*/
	GC		gch;	/*  G0ＧＣ		*/
	GC		gck;	/*  G1ＧＣ		*/
	GC		gcr;	/*  G2ＧＣ		*/
	GC		gcg;	/*  G3ＧＣ			*/
	GC		reversegch;/*  G0用反転ＧＣ	*/
	GC		reversegck;/*  G1用反転ＧＣ	*/
	GC		reversegcr;/*  G2用反転ＧＣ	*/
	GC		reversegcg;/*  G3反転ＧＣ		*/
	long		c0;	/*  １行目変換行のカラム数	*/
	long		c1;	/*  ２行目変換行のカラム数	*/
	long		c2;	/*  ３行目変換行のカラム数	*/
	long		linefeed1;/*  １行目変換行行末処理用フラグ	*/
	long		linefeed2;/*  ２行目変換行行末処理用フラグ	*/
	int		max_l1;
	int		max_l2;
	int		max_l3;
	long		re_count;/* 同じ変換ウィンドウに、何人アクセスしてるか*/
	long		vst;	/*  表示されている最初のカラム位置
				    c_b->vst の代わりになるものです	*/
	int		note_fore;  /*  前がチョン切れている印を
					1 : 表示している  0 : してない	*/
	int		note_back;  /*  後がチョン切れている印を
					1 : 表示している  0 : してない	*/
	int		fd;	/* ソケットによるダイレクトインタフェース用
				   ファイルディスクリプタ	*/
	int		*buf0;
	int		firstx;
	int		firsty;
	long		currentcol0;/* バファ内でのカーソル位置	 */
	int		cursor_flag0;
	int		cursor_flag_sv0;
	int		mark_flag0;
	int		max_pos0;
	int		*buf;
	int		cursor_flag;
	int		mark_flag;
	int		u_line_flag;
	int		r_flag;
	int		b_flag;		/* V4.0 */
	int		max_pos;
	int		currentcol;
	int		maxcolumns;
	int		visible;
	int		visible_line;
	int		max_cur;
	int		m_cur_flag;
	int		n_write;
	int		del_x;
	struct wnmo_client *w_c;
} XJClient;

typedef struct _YesOrNo {
	Window  w;
	Window  title_w;
	Window  yes_w;
	Window  no_w;
	unsigned char *title;
	unsigned char *yes_t;
	unsigned char *no_t;
} YesOrNo;

typedef struct _Inspect {
	Window	w;
	Window	w1;
	Window	title_w;
	Window	cancel_w;
	Window	del_w;
	Window	use_w;
	Window	next_w;
	Window	back_w;
	unsigned char *msg;
	unsigned char *title;
	unsigned char *cancel;
	unsigned char *del_t;
	unsigned char *use_t;
	unsigned char *next;
	unsigned char *back;
} Inspect;

/* 一覧表示メニューのための構造体 */
typedef struct	_Ichiran {
	Window		w;	/* タイトルなどを含む親ウィンドウ	*/
	Window		w1;	/* 一覧表示する子ウィンドウ	*/
	Window		title_w;/* タイトル用ウィンドウ		*/
	Window		subtitle_w; /* サブタイトル用ウィンドウ	*/
	Window		exec_title_w; /* 実行タイトル用ウィンドウ*/
	Window		cancel_w; /* キャンセルタイトル用ウィンドウ	*/
	Window		next_w;	 /*次ページタイトル用ウィンドウ	*/
	Window		back_w;	 /*前ページタイトル用ウィンドウ	*/
	Window		mouse_w;
	GC		invertgc;/* ハイライト表示用ＧＣ	*/
	Window		nyuu_w;/* 入力用ウィンドウ*/
	int		kosuu;	/* 一覧表示の個数	*/
	unsigned char	*buf[200];/* セーブ用バッファ	*/
	unsigned char	*title;	/* タイトル文字列	*/
	unsigned char	*subtitle; /* サブタイトル文字列	*/
	unsigned char	*exec_title; /* 実行タイトル文字列	*/
	unsigned char	*cancel;/*キャンセルタイトル文字列	*/
	unsigned char	*next;	 /*次ページタイトル文字列	*/
	unsigned char	*back;	 /*前ページタイトル文字列	*/
	unsigned char	*mouse;
	int		mode;	/* 一覧表のモード( sdefine.h 参照のこと)*/
} Ichiran;


/* スクリーン下のｘｗｎｍｏウィンドウのための構造体 */
typedef struct  _JFEWSTR {
	XJClient	*xjclient;/*  デフォルトウィンドウの XJClient	*/
	XJClient	*jutilxjc;/*  デフォルトウィンドウの XJClient	*/
	Window		title;	/*  変換off時のタイトル用ウィンドウ	*/
	Ichiran		*ichi;	/* 一覧表示用構造体		*/
	YesOrNo		*yes_no;
	Inspect		*inspect;
	XJClient	*j_c;
	int		sel_ret;
} JFEWSTR;
	

/* キー入力ウィンドウのための構造体
   XJp_begin()が呼ばれると作られる */
typedef struct	_XJInput {
	Window		w;	/*  キー入力を受け付けるウィンドウ	*/
	XJClient 	*pclient;/*  表示するクライアントの wnmo_client */
	int		save_event;/*  キーイベントを 1 : セーブしてるか
						      2 : してないか	*/
	XEvent		event;	/*  セーブしているキーイベント		*/
} XJInput;

typedef struct _DIC_SERV_INFO {
	char jserver_name[256];
	char uumrc_name[256];
	char uumkey_name[256];
	char rkfile_name[256];
	char cvtfun_name[256];
	char cvtmeta_name[256];
	char user_name[256];
	XID ascii_font_id;
	XID kanji_font_id;
	XID kana_font_id;
	int fore_ground;
	int back_ground;
} DIC_SERV_INFO;

struct User_uumrc{
	char user_name[256];
	char uumrc_name[256];
};

#define DICSERVERNAME	"/usr/local/bin/Wnn4/dicserver"
#define DICSERVER	"DICSERVER"
#define DICENV		"DICENV"

#define BUFSIZE 512
#define STRBUFSIZE 512
#define SS2 0x8e
#define SS3 0x8f

extern Display	*dpy;
extern JFEWSTR	*jfew;
extern XJClient	*cur_x;
extern XJClient	*cur_p;
extern XJClient	*client_mem[MAX_CLIENTS];
extern int	client_num;
extern int	input_num;
extern XJInput	*input_mem[MAX_CLIENT];
extern Atom	select_id;
extern Atom	select_buf[10];

#define FontWidth(xjc)		(xjc->h_f->max_bounds.width)
#define FontHeight(xjc)		(xjc->h_f->ascent + xjc->h_f->descent)
#define FontAscent(xjc)		(xjc->h_f->ascent)

#define REV	(1<<8)
#define BOLD	(1<<9)		/* add by KUWA V4.0 */
#define UNDER	(1<<10)
#define KANA	(1<<12)
#define KAN1	(1<<13)
#define	KAN2	(1<<14)

#define	DEFANKFONT	"roman"
#define	DEFKANJIFONT	"kanji"
#define	DEFKANAFONT	"kana"

/*クライアントから JP_frontend に送られるコマンド*/

#define	XJP_OPEN	1  /*クライアント登録要求(XJp_open)*/
#define	XJP_CLOSE	2  /*終了要求(XJp_Close)*/
#define	XJP_BEGIN	3  /*キーの開放通知(XJp_Begin)*/
#define	XJP_END		4  /*キーの獲得要求(XJp_End)*/
#define	XJP_VISIBLE	5  /*変換開始要求１(XJp_Visible)*/
#define XJP_INVISIBLE	6  /*変換終了要求 (XJp_Invisible)*/
#define	XJP_CHANGE	7  /*変更要求２(XJp_Change)*/
#define	XJP_MOVE	8

/*JP_frontendから クライアントに送られるコマンド*/

#define	XJP_WINDOWID	1
#define	XJP_ERROR	2


#define	DIC_ADD		1
#define	DIC_KENSAKU	2
#define	DIC_ICHIRAN	3
#define	DIC_PARAM	4
#define	DIC_INFO	5
#define	DIC_FZK		6
#define	DIC_TOUROKU	7

#define	DIC_START	1000

#ifndef XK_Eisuu_Lock
#define	XK_Eisuu_Lock	0xFF2E
#endif

/* Property 名 */
#define	XJPCLIENT	XJPCLIENT
#define	XJPFRONTEND	XJPFRONTEND

extern unsigned char	*kanji_in_start_msg;
extern unsigned char	*kanji_in_end_msg;
extern unsigned char	*yomi_in_msg;
extern unsigned char	*hinsi_in_msg;
extern unsigned char	*jisho_int_msg;

#define		XWNMO_TITLE		"かな漢字変換:"
#define		XWNMO_RCENV	"XWNMORC"
#define		USR_XWNMORC	"/.jfewrc"
#define		XWNMORCFILE	"/usr/local/lib/wnn/jfewrc"

#define select_return		(jfew->sel_ret)

#define	MESSAGE_X	500
#define	MESSAGE_Y	800

#define	malloc	Salloc
extern char *Salloc();
