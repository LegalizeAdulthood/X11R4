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

#define XWNMO_GETOPTSTR	"hHk:r:D:n:v:F:M"
#define XWNMO_OPTIONS	"hHkrDnvFM"
#define WNMO_FRAGMENT_NAME      "WNMOFRAG"
#ifdef nodef
#ifdef SX8870
#       define DRAW_MOUSE
#       define SYSVR2
#ifndef MACHINE_M
#       define SOCKET
#endif
/*#       define TERMINFO */
#endif
#endif

#define MAXFONTNMLEN	256	/* フォント名文字列	*/
#define IN_BORDER	2
#define JCLIENTS	4

typedef	struct	_XJClient {
	Window	w;		/*  かな漢字変換要求ウィンドウＩＤ	*/
	long	p_width;	/*  かな漢字変換要求ウィンドウの大きさ
							( ピクセル値 )	*/
	long	p_height;	/*  かな漢字変換要求ウィンドウの大きさ
							( ピクセル値 )	*/
	long	x;		/*  かな漢字変換ウィンドウの位置( ピクセル値 )*/
	long	y;		/*  かな漢字変換ウィンドウの位置( ピクセル値 )*/
	long	width;		/*  かな漢字変換ウィンドウの幅( ピクセル値 )  */
	long	height;		/*  かな漢字変換ウィンドウの高さ( ピクセル値 )*/
	long	bp;		/*  変換ウィンドウの境界幅( ピクセル単位 )*/
	Window		wp1;	/*  ダミーの親ウィンドウ		*/
	Window		w1;	/*  実際に文字が書かれるウィンドウ(１行目)*/
	Window		wnf;	/*  前がチョン切れている印用ウィンドウ	*/
	Window		wnb;	/*  後がチョン切れている印用ウィンドウ  */
	Window		wnn;	/*  半分チョン切れてしまった文字を消すための
				    ウィンドウ				*/
	Window		wp;
	long		max_columns;/*  表示できる最大カラム数	*/
	long		vst;	/*  表示されている最初のカラム位置
				    c_b->vst の代わりになるものです	*/
	int		note_fore;  /*  前がチョン切れている印を
					1 : 表示している  0 : してない	*/
	int		note_back;  /*  後がチョン切れている印を
					1 : 表示している  0 : してない	*/
	int		firstx;
	int		firsty;
	int		*buf;
	int		cursor_flag;
	int		mark_flag;
	int		u_line_flag;
	int		r_flag;
	int		b_flag;		/* V4.0 */
	int		max_pos;
	int		currentcol;
	int		maxcolumns;
} XJClient;

/* 辞書操作の時に使用するクライアントモドキなもの */
typedef struct	_Jclient {
	XJClient	*cl;	/*  XJClient 構造体	*/
	int		buf[128];/*  表示文字列をセーブするバッファ	*/
} Jclient;

typedef struct _YesOrNo {
	Window	w;
	Window	title_w;
	Window	yes_w;
	Window	no_w;
	unsigned char *title;
	unsigned char *yes_t;
	unsigned char *no_t;
} YesOrNo;

typedef struct _Keytable {
	int	cnt;
	Window	w;
	Window	key_w[3];
	unsigned char *key_t[3];
} Keytable;
/* 一覧表示メニューのための構造体 */
typedef struct	_Ichiran {
	Window		w;	/* タイトルなどを含む親ウィンドウ	*/
	Window		w1;	/* 一覧表示する子ウィンドウ	*/
	Window		title_w;/* タイトル用ウィンドウ		*/
	Window		comment_w;
	Window		subtitle_w; /* サブタイトル用ウィンドウ */
	Window		cancel_w; /* キャンセルタイトル用ウィンドウ	*/
	Window		exec_title_w; /* 実行タイトル用ウィンドウ*/
	Window		next_w;	 /* 次ページタイトル用ウィンドウ	*/
	Window		back_w;	 /* 前ページタイトル用ウィンドウ	*/
	Window  rk_mode_w;      /*  romkan mode Window          */
	GC		invertgc;/* ハイライト表示用ＧＣ	*/
	Jclient	*nyuuryoku_jutil;/* 入力用ウィンドウ構造体      */
	int		kosuu;	/* 一覧表示の個数	*/
	unsigned char	*buf[200];/* セーブ用バッファ	*/
	unsigned char	*comment;
	unsigned char	*title;	/* タイトル文字列	*/
	unsigned char	*subtitle; /* サブタイトル文字列        */
	unsigned char	*cancel;/*キャンセルタイトル文字列	*/
	unsigned char	*exec_title; /* 実行タイトル文字列      */
	unsigned char	*next;	 /* 次ページタイトル文字列	*/
	unsigned char	*back;	 /* 前ページタイトル文字列	*/
	unsigned char	*rk_mode; /* romkan mode */
	Keytable	*keytable;
} Ichiran;


/* 辞書登録のための構造体 */
typedef struct	_Jutil {
	Window	w;		/*  タイトルなどを含む親ウィンドウ	*/
	Window	rk_mode_w;	/*  romkan mode Window		*/
	Window	title_w;	/*  タイトル用ウィンドウ	*/
	Window	cancel_w;	/*  キャンセルタイトル用ウィンドウ	*/
	Window	exec_w;		/*  実行タイトル用ウィンドウ	*/
	Window	mes_w[JCLIENTS];	/*  タイトル用ウィンドウ	*/
	unsigned char	*title;		/*  タイトル文字列	*/
	unsigned char	*cancel;	/*  キャンセルタイトル文字列	*/
	unsigned char	*exec;		/*  実行タイトル文字列	*/
	unsigned char	*mes_title[JCLIENTS];	/*  タイトル文字列	*/
	Jclient	*mes_jutil[JCLIENTS];	/*  漢字入力 Jutil	*/
	int	mode;
} Jutil;


/* スクリーン下のｘｗｎｍｏウィンドウのための構造体 */
typedef struct  _JFEWSTR {
	XJClient	*xjclient;/*  デフォルトウィンドウの XJClient	*/
	Ichiran		*ichi;	/* 一覧表示用構造体		*/
	Jutil		*jutil;/*  辞書用構造体		*/
	YesOrNo		*yes_no;
	int		mode;	/* ICHIRAN or JUTIL */
	int		fg;
	int		bg;
	XFontStruct	*h_f;	/*  半角文字フォントの構造体	*/
	XFontStruct	*k_f;	/*  全角文字フォントの構造体	*/
	XFontStruct	*r_f;	/*  kana文字フォントの構造体	*/
	XFontStruct	*g_f;	/*  外字フォントの構造体	*/
	GC		gch;	/*  半角文字用ＧＣ		*/
	GC		gck;	/*  全角文字用ＧＣ		*/
	GC		gcr;	/*  kana文字用ＧＣ		*/
	GC		gcg;	/*  外字用ＧＣ			*/
	GC		reversegch;/*  半角文字用反転ＧＣ	*/
	GC		reversegck;/*  全角文字用反転ＧＣ	*/
	GC		reversegcr;/*  kana文字用反転ＧＣ	*/
	GC		reversegcg;/*  外字用反転ＧＣ		*/
} JFEWSTR;
	
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

#define BUFSIZE 512
#define STRBUFSIZE 512
#define SS2 0x8e

extern Display	*dpy;
extern int	screen;
extern JFEWSTR	*jfew;
extern Atom	select_id;
extern int	*cur_JW_buf;

#define FontWidth		(jfew->h_f->max_bounds.width)
#define FontHeight		(jfew->h_f->ascent + jfew->h_f->descent)
#define FontAscent		(jfew->h_f->ascent)

#define REV	(1<<8)
#define BOLD	(1<<9)		/* add by KUWA V4.0 */
#define UNDER	(1<<10)
#define KANA	(1<<12)
#define KAN1	(1<<13)
#define	KAN2	(1<<14)

#ifndef XK_Eisuu_Lock
#define	XK_Eisuu_Lock	0xFF2E
#endif

/* Property 名 */
#define	DICSERVER	"DICSERVER"
#define	DICENV		"DICENV"

extern Jclient *cur_jutil;

#define	DIC_ADD		1
#define	DIC_KENSAKU	2
#define	DIC_ICHIRAN	3
#define	DIC_PARAM	4
#define	DIC_INFO	5
#define	DIC_FZK		6
#define	DIC_TOUROKU	7

#define	DIC_START	1000

#define	JUTIL_X		500
#define	JUTIL_Y		500
#define	MESSAGE_X	500
#define	MESSAGE_Y	800

#define malloc  Salloc
extern char *Salloc();
