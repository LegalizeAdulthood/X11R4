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

	�أ��������б��ףΣͣϡ��إå�

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

#define MAXFONTNMLEN	256	/* �ե����̾ʸ����	*/
#define IN_BORDER	2
#define JCLIENTS	4

typedef	struct	_XJClient {
	Window	w;		/*  ���ʴ����Ѵ��׵ᥦ����ɥ��ɣ�	*/
	long	p_width;	/*  ���ʴ����Ѵ��׵ᥦ����ɥ����礭��
							( �ԥ������� )	*/
	long	p_height;	/*  ���ʴ����Ѵ��׵ᥦ����ɥ����礭��
							( �ԥ������� )	*/
	long	x;		/*  ���ʴ����Ѵ�������ɥ��ΰ���( �ԥ������� )*/
	long	y;		/*  ���ʴ����Ѵ�������ɥ��ΰ���( �ԥ������� )*/
	long	width;		/*  ���ʴ����Ѵ�������ɥ�����( �ԥ������� )  */
	long	height;		/*  ���ʴ����Ѵ�������ɥ��ι⤵( �ԥ������� )*/
	long	bp;		/*  �Ѵ�������ɥ��ζ�����( �ԥ�����ñ�� )*/
	Window		wp1;	/*  ���ߡ��οƥ�����ɥ�		*/
	Window		w1;	/*  �ºݤ�ʸ�����񤫤�륦����ɥ�(������)*/
	Window		wnf;	/*  ����������ڤ�Ƥ�����ѥ�����ɥ�	*/
	Window		wnb;	/*  �夬������ڤ�Ƥ�����ѥ�����ɥ�  */
	Window		wnn;	/*  Ⱦʬ������ڤ�Ƥ��ޤä�ʸ����ä������
				    ������ɥ�				*/
	Window		wp;
	long		max_columns;/*  ɽ���Ǥ�����祫����	*/
	long		vst;	/*  ɽ������Ƥ���ǽ�Υ�������
				    c_b->vst ������ˤʤ��ΤǤ�	*/
	int		note_fore;  /*  ����������ڤ�Ƥ������
					1 : ɽ�����Ƥ���  0 : ���Ƥʤ�	*/
	int		note_back;  /*  �夬������ڤ�Ƥ������
					1 : ɽ�����Ƥ���  0 : ���Ƥʤ�	*/
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

/* �������λ��˻��Ѥ��륯�饤����ȥ�ɥ��ʤ�� */
typedef struct	_Jclient {
	XJClient	*cl;	/*  XJClient ��¤��	*/
	int		buf[128];/*  ɽ��ʸ����򥻡��֤���Хåե�	*/
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
/* ����ɽ����˥塼�Τ���ι�¤�� */
typedef struct	_Ichiran {
	Window		w;	/* �����ȥ�ʤɤ�ޤ�ƥ�����ɥ�	*/
	Window		w1;	/* ����ɽ������ҥ�����ɥ�	*/
	Window		title_w;/* �����ȥ��ѥ�����ɥ�		*/
	Window		comment_w;
	Window		subtitle_w; /* ���֥����ȥ��ѥ�����ɥ� */
	Window		cancel_w; /* ����󥻥륿���ȥ��ѥ�����ɥ�	*/
	Window		exec_title_w; /* �¹ԥ����ȥ��ѥ�����ɥ�*/
	Window		next_w;	 /* ���ڡ��������ȥ��ѥ�����ɥ�	*/
	Window		back_w;	 /* ���ڡ��������ȥ��ѥ�����ɥ�	*/
	Window  rk_mode_w;      /*  romkan mode Window          */
	GC		invertgc;/* �ϥ��饤��ɽ���ѣǣ�	*/
	Jclient	*nyuuryoku_jutil;/* �����ѥ�����ɥ���¤��      */
	int		kosuu;	/* ����ɽ���θĿ�	*/
	unsigned char	*buf[200];/* �������ѥХåե�	*/
	unsigned char	*comment;
	unsigned char	*title;	/* �����ȥ�ʸ����	*/
	unsigned char	*subtitle; /* ���֥����ȥ�ʸ����        */
	unsigned char	*cancel;/*����󥻥륿���ȥ�ʸ����	*/
	unsigned char	*exec_title; /* �¹ԥ����ȥ�ʸ����      */
	unsigned char	*next;	 /* ���ڡ��������ȥ�ʸ����	*/
	unsigned char	*back;	 /* ���ڡ��������ȥ�ʸ����	*/
	unsigned char	*rk_mode; /* romkan mode */
	Keytable	*keytable;
} Ichiran;


/* ������Ͽ�Τ���ι�¤�� */
typedef struct	_Jutil {
	Window	w;		/*  �����ȥ�ʤɤ�ޤ�ƥ�����ɥ�	*/
	Window	rk_mode_w;	/*  romkan mode Window		*/
	Window	title_w;	/*  �����ȥ��ѥ�����ɥ�	*/
	Window	cancel_w;	/*  ����󥻥륿���ȥ��ѥ�����ɥ�	*/
	Window	exec_w;		/*  �¹ԥ����ȥ��ѥ�����ɥ�	*/
	Window	mes_w[JCLIENTS];	/*  �����ȥ��ѥ�����ɥ�	*/
	unsigned char	*title;		/*  �����ȥ�ʸ����	*/
	unsigned char	*cancel;	/*  ����󥻥륿���ȥ�ʸ����	*/
	unsigned char	*exec;		/*  �¹ԥ����ȥ�ʸ����	*/
	unsigned char	*mes_title[JCLIENTS];	/*  �����ȥ�ʸ����	*/
	Jclient	*mes_jutil[JCLIENTS];	/*  �������� Jutil	*/
	int	mode;
} Jutil;


/* �����꡼�󲼤Σ������復����ɥ��Τ���ι�¤�� */
typedef struct  _JFEWSTR {
	XJClient	*xjclient;/*  �ǥե���ȥ�����ɥ��� XJClient	*/
	Ichiran		*ichi;	/* ����ɽ���ѹ�¤��		*/
	Jutil		*jutil;/*  �����ѹ�¤��		*/
	YesOrNo		*yes_no;
	int		mode;	/* ICHIRAN or JUTIL */
	int		fg;
	int		bg;
	XFontStruct	*h_f;	/*  Ⱦ��ʸ���ե���Ȥι�¤��	*/
	XFontStruct	*k_f;	/*  ����ʸ���ե���Ȥι�¤��	*/
	XFontStruct	*r_f;	/*  kanaʸ���ե���Ȥι�¤��	*/
	XFontStruct	*g_f;	/*  �����ե���Ȥι�¤��	*/
	GC		gch;	/*  Ⱦ��ʸ���ѣǣ�		*/
	GC		gck;	/*  ����ʸ���ѣǣ�		*/
	GC		gcr;	/*  kanaʸ���ѣǣ�		*/
	GC		gcg;	/*  �����ѣǣ�			*/
	GC		reversegch;/*  Ⱦ��ʸ����ȿž�ǣ�	*/
	GC		reversegck;/*  ����ʸ����ȿž�ǣ�	*/
	GC		reversegcr;/*  kanaʸ����ȿž�ǣ�	*/
	GC		reversegcg;/*  ������ȿž�ǣ�		*/
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

/* Property ̾ */
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
