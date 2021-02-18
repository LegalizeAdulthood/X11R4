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
#include "wnmoclient.h"

#define XWNMO_GETOPTSTR	"hHk:r:D:n:vF:M:"
#define XWNMO_OPTIONS	"hHkrDnvFM"
#define WNMO_FRAGMENT_NAME      "WNMOFRAG"

#define MAX_CLIENT	32	/* ��³�Ǥ��륯�饤����Ȥκ���� */
#define MAX_SELECT	32	/* ��³�Ǥ��륯�饤����Ȥκ����
				   (�����åȥ��󥿥ե�������)     */
#define MAXFONTNMLEN	256	/* �ե����̾ʸ����	*/
#define IN_BORDER	2

typedef	struct	_XJClient {
	long	dispmode;	/*  ���ʴ����Ѵ�������ɥ��η���	*/
				/*  0 : �ƥ�����ɥ��β���
				    1 : �ƥ����������
				    2 : �����꡼����(�ǥե����)	*/
	unsigned char a_data[128];	/*  �桼��̾ (V3.1 �ޤǤ�̤����)*/
	Window	w;		/*  ���ʴ����Ѵ��׵ᥦ����ɥ��ɣ�	*/
	unsigned long	mask;	/*  ��°���Υޥ�����			*/
	long	p_width;	/*  ���ʴ����Ѵ��׵ᥦ����ɥ����礭��
							( �ԥ������� )	*/
	long	p_height;	/*  ���ʴ����Ѵ��׵ᥦ����ɥ����礭��
							( �ԥ������� )	*/
	long	x;		/*  ���ʴ����Ѵ�������ɥ��ΰ���( �ԥ������� )*/
	long	y;		/*  ���ʴ����Ѵ�������ɥ��ΰ���( �ԥ������� )*/
	long	width;		/*  ���ʴ����Ѵ�������ɥ�����( �ԥ������� )  */
	long	height;		/*  ���ʴ����Ѵ�������ɥ��ι⤵( �ԥ������� )*/
	unsigned long	fg;	/*  ���ʴ����Ѵ�������ɥ��Υե��������ɿ�  */
	unsigned long	bg;	/*  ���ʴ����Ѵ�������ɥ��ΥХå������ɿ�  */
	long	bp;		/*  �Ѵ�������ɥ��ζ�����( �ԥ�����ñ�� )*/
	long	status_x;
	long	status_y;
	char	h_font[256];	/* ���ʴ����Ѵ�������ɥ�G0�ե����̾*/
	char	h_escape[4];
	char	k_font[256];	/* ���ʴ����Ѵ�������ɥ�G1�ե����̾*/
	char	k_escape[4];
	char	r_font[256];	/* ���ʴ����Ѵ�������ɥ�G2�ե����̾*/
	char	r_escape[4];
	char	g_font[256];	/* ���ʴ����Ѵ�������ɥ�G3�ե����̾	*/
	char	g_escape[4];
	Window		wp1;	/*  ���ߡ��οƥ�����ɥ�		*/
	Window		wp2;	/*  ���Υ�����ɥ��ξ��ҥ�����ɥ�(w1,w2,w3)*/
	Window		wp3;    /*  ��ư���ơ�ʸ����ɽ�����롣		*/
	Window		w1;	/*  �ºݤ�ʸ�����񤫤�륦����ɥ�(������)*/
	Window		w2;	/*  �ºݤ�ʸ�����񤫤�륦����ɥ�(������)*/
	Window		w3;	/*  �ºݤ�ʸ�����񤫤�륦����ɥ�(������)*/
	Window		wnf;	/*  ����������ڤ�Ƥ�����ѥ�����ɥ�	*/
	Window		wnb;	/*  �夬������ڤ�Ƥ�����ѥ�����ɥ�  */
	Window		wnn;	/*  Ⱦʬ������ڤ�Ƥ��ޤä�ʸ����ä������
				    ������ɥ�				*/
	Window		wp;
	Window		wt;
	Window		ws;
	long		status_lock;
	long		columns;/*  �Ѵ�������ɥ�����������
				    (�ǥե���ȤΥ�����ɥ������ȤäƤ��ʤ���)*/
	long		max_columns;/*  ɽ���Ǥ�����祫����	*/
	XFontStruct	*h_f;	/*  G0�ե���Ȥι�¤��	*/
	XFontStruct	*k_f;	/*  G1�ե���Ȥι�¤��	*/
	XFontStruct	*r_f;	/*  G2�ե���Ȥι�¤��	*/
	XFontStruct	*g_f;	/*  G3�ե���Ȥι�¤��	*/
	GC		gch;	/*  G0�ǣ�		*/
	GC		gck;	/*  G1�ǣ�		*/
	GC		gcr;	/*  G2�ǣ�		*/
	GC		gcg;	/*  G3�ǣ�			*/
	GC		reversegch;/*  G0��ȿž�ǣ�	*/
	GC		reversegck;/*  G1��ȿž�ǣ�	*/
	GC		reversegcr;/*  G2��ȿž�ǣ�	*/
	GC		reversegcg;/*  G3ȿž�ǣ�		*/
	long		c0;	/*  �������Ѵ��ԤΥ�����	*/
	long		c1;	/*  �������Ѵ��ԤΥ�����	*/
	long		c2;	/*  �������Ѵ��ԤΥ�����	*/
	long		linefeed1;/*  �������Ѵ��Թ��������ѥե饰	*/
	long		linefeed2;/*  �������Ѵ��Թ��������ѥե饰	*/
	int		max_l1;
	int		max_l2;
	int		max_l3;
	long		re_count;/* Ʊ���Ѵ�������ɥ��ˡ����ͥ����������Ƥ뤫*/
	long		vst;	/*  ɽ������Ƥ���ǽ�Υ�������
				    c_b->vst ������ˤʤ��ΤǤ�	*/
	int		note_fore;  /*  ����������ڤ�Ƥ������
					1 : ɽ�����Ƥ���  0 : ���Ƥʤ�	*/
	int		note_back;  /*  �夬������ڤ�Ƥ������
					1 : ɽ�����Ƥ���  0 : ���Ƥʤ�	*/
	int		fd;	/* �����åȤˤ������쥯�ȥ��󥿥ե�������
				   �ե�����ǥ�������ץ�	*/
	int		*buf0;
	int		firstx;
	int		firsty;
	long		currentcol0;/* �Хե���ǤΥ����������	 */
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

/* ����ɽ����˥塼�Τ���ι�¤�� */
typedef struct	_Ichiran {
	Window		w;	/* �����ȥ�ʤɤ�ޤ�ƥ�����ɥ�	*/
	Window		w1;	/* ����ɽ������ҥ�����ɥ�	*/
	Window		title_w;/* �����ȥ��ѥ�����ɥ�		*/
	Window		subtitle_w; /* ���֥����ȥ��ѥ�����ɥ�	*/
	Window		exec_title_w; /* �¹ԥ����ȥ��ѥ�����ɥ�*/
	Window		cancel_w; /* ����󥻥륿���ȥ��ѥ�����ɥ�	*/
	Window		next_w;	 /*���ڡ��������ȥ��ѥ�����ɥ�	*/
	Window		back_w;	 /*���ڡ��������ȥ��ѥ�����ɥ�	*/
	Window		mouse_w;
	GC		invertgc;/* �ϥ��饤��ɽ���ѣǣ�	*/
	Window		nyuu_w;/* �����ѥ�����ɥ�*/
	int		kosuu;	/* ����ɽ���θĿ�	*/
	unsigned char	*buf[200];/* �������ѥХåե�	*/
	unsigned char	*title;	/* �����ȥ�ʸ����	*/
	unsigned char	*subtitle; /* ���֥����ȥ�ʸ����	*/
	unsigned char	*exec_title; /* �¹ԥ����ȥ�ʸ����	*/
	unsigned char	*cancel;/*����󥻥륿���ȥ�ʸ����	*/
	unsigned char	*next;	 /*���ڡ��������ȥ�ʸ����	*/
	unsigned char	*back;	 /*���ڡ��������ȥ�ʸ����	*/
	unsigned char	*mouse;
	int		mode;	/* ����ɽ�Υ⡼��( sdefine.h ���ȤΤ���)*/
} Ichiran;


/* �����꡼�󲼤Σ������復����ɥ��Τ���ι�¤�� */
typedef struct  _JFEWSTR {
	XJClient	*xjclient;/*  �ǥե���ȥ�����ɥ��� XJClient	*/
	XJClient	*jutilxjc;/*  �ǥե���ȥ�����ɥ��� XJClient	*/
	Window		title;	/*  �Ѵ�off���Υ����ȥ��ѥ�����ɥ�	*/
	Ichiran		*ichi;	/* ����ɽ���ѹ�¤��		*/
	YesOrNo		*yes_no;
	Inspect		*inspect;
	XJClient	*j_c;
	int		sel_ret;
} JFEWSTR;
	

/* �������ϥ�����ɥ��Τ���ι�¤��
   XJp_begin()���ƤФ��Ⱥ���� */
typedef struct	_XJInput {
	Window		w;	/*  �������Ϥ�����դ��륦����ɥ�	*/
	XJClient 	*pclient;/*  ɽ�����륯�饤����Ȥ� wnmo_client */
	int		save_event;/*  �������٥�Ȥ� 1 : �����֤��Ƥ뤫
						      2 : ���Ƥʤ���	*/
	XEvent		event;	/*  �����֤��Ƥ��륭�����٥��		*/
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

/*���饤����Ȥ��� JP_frontend �������륳�ޥ��*/

#define	XJP_OPEN	1  /*���饤�������Ͽ�׵�(XJp_open)*/
#define	XJP_CLOSE	2  /*��λ�׵�(XJp_Close)*/
#define	XJP_BEGIN	3  /*�����γ�������(XJp_Begin)*/
#define	XJP_END		4  /*�����γ����׵�(XJp_End)*/
#define	XJP_VISIBLE	5  /*�Ѵ������׵ᣱ(XJp_Visible)*/
#define XJP_INVISIBLE	6  /*�Ѵ���λ�׵� (XJp_Invisible)*/
#define	XJP_CHANGE	7  /*�ѹ��׵ᣲ(XJp_Change)*/
#define	XJP_MOVE	8

/*JP_frontend���� ���饤����Ȥ������륳�ޥ��*/

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

/* Property ̾ */
#define	XJPCLIENT	XJPCLIENT
#define	XJPFRONTEND	XJPFRONTEND

extern unsigned char	*kanji_in_start_msg;
extern unsigned char	*kanji_in_end_msg;
extern unsigned char	*yomi_in_msg;
extern unsigned char	*hinsi_in_msg;
extern unsigned char	*jisho_int_msg;

#define		XWNMO_TITLE		"���ʴ����Ѵ�:"
#define		XWNMO_RCENV	"XWNMORC"
#define		USR_XWNMORC	"/.jfewrc"
#define		XWNMORCFILE	"/usr/local/lib/wnn/jfewrc"

#define select_return		(jfew->sel_ret)

#define	MESSAGE_X	500
#define	MESSAGE_Y	800

#define	malloc	Salloc
extern char *Salloc();
