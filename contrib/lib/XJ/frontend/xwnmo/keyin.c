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
#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include "xwnmo.h"
#include "XJp_errno.h"
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#ifdef UX386
#include <net/in.h>		/* htonl() の定義のため			*/
#endif

#define	XOR(a,b)	((a&(~b)) | ((~a)&b))

int	ret;
Display	*dpy = 0;			/* ディスプレイ構造体へのポインタ */
XJClient *client_mem[MAX_CLIENT];/* 登録されているクライアントに対する
				    XJClient 構造体へのポインタの一欄 */
XJClient *cur_x;		/* カレントのクライアントの XJClient */
XJClient *cur_p;		/* カレントのクライアントの XJClient */
int	client_num;		/* 登録されているクライアントの数 */
int	input_num;		/* キー入力を受け付けているウィンドウ数 */
XJInput *input_mem[MAX_CLIENT];	/* キー入力を受け付けているウィンドウに対する
					   XJInput 構造体へのポインタの一欄 */
XJInput *cur_input;		/* カレントの入力ウィンドウの XJInput */
JFEWSTR	*jfew;		 	/* 自分自身の JFEWSTR 		*/

Romkan	*cur_rk;

Atom	select_id;		/* JFEW の ID 用 プロパティ	*/
Atom	motif_id = 0;	/* MOTIF */
Atom	motif_id1 = 0;	/* MOTIF */

unsigned long boderground;
Cursor	cursor1, cursor2, cursor3;


Window	dicserver_id = -1;
DIC_SERV_INFO *dic_info;

int	button_x;
int	button_y;
int	syuuryou = 0;
unsigned char	ret_sockbuf[STRBUFSIZE];
extern	int	select_button;
static	char	*icon_name = "xwnmo";
int iconic_start = 0;

unsigned char *buffer1;
unsigned char *buffer2;

#define ic_width 50
#define ic_height 50
static char ic_bits[] = {
   0xff, 0x03, 0x00, 0x00, 0x00, 0x80, 0x03, 0xfe, 0x07, 0x00, 0x00, 0x00,
   0xc0, 0x01, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0xe0, 0x00, 0xfc, 0x0f, 0x00,
   0x00, 0x00, 0xe0, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x70, 0x00, 0xf0,
   0x3f, 0x00, 0x00, 0x00, 0x38, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x1c,
   0x00, 0xc0, 0xff, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x80, 0xff, 0x01, 0x00,
   0x00, 0x07, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x07, 0x00, 0x00, 0xff,
   0x03, 0x00, 0x80, 0x03, 0x00, 0x00, 0xfe, 0x07, 0x00, 0xc0, 0x01, 0x00,
   0x00, 0xfc, 0x0f, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x70,
   0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x70, 0x00, 0x00, 0x00, 0xf0, 0x3f,
   0x00, 0x38, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x1c, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xdf, 0xc1, 0x3d,
   0x3e, 0x7e, 0x00, 0xc6, 0x88, 0x81, 0x18, 0x1c, 0x81, 0x01, 0xc6, 0x88,
   0x83, 0x38, 0x9a, 0x00, 0x03, 0xcc, 0x89, 0x86, 0xe8, 0x99, 0x00, 0x03,
   0xac, 0x85, 0x8c, 0xc8, 0x98, 0x00, 0x03, 0xac, 0x85, 0x98, 0xe8, 0x99,
   0x00, 0x03, 0xac, 0x85, 0xb0, 0x08, 0x98, 0x00, 0x03, 0x18, 0x83, 0xe0,
   0x08, 0x98, 0x00, 0x03, 0x18, 0x83, 0xc0, 0x08, 0x18, 0x81, 0x01, 0xfc,
   0xc7, 0xc1, 0x1d, 0x3c, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x70,
   0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x38, 0x00, 0xe0, 0x7f, 0x00, 0x00,
   0x00, 0x38, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x1c, 0x00, 0xc0, 0xff,
   0x00, 0x00, 0x00, 0x0e, 0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x07, 0x00,
   0x00, 0xff, 0x03, 0x00, 0x80, 0x03, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x80,
   0x03, 0x00, 0x00, 0xfe, 0x07, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xfc, 0x0f,
   0x00, 0xe0, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x70, 0x00, 0x00, 0x00,
   0xf0, 0x3f, 0x00, 0x38, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x1c, 0x00,
   0x00, 0x00, 0xc0, 0xff, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00,
   0x0e, 0x00, 0x00, 0x00, 0x80, 0xff, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00,
   0xff, 0x03};
/******************************************************************************
	ＸＷＮＭＯウィンドウ作成 ルーチン
			from main()
******************************************************************************/
create_jfew()
{
	register XJClient	*xjc, *jutilxjc;
	register Ichiran	*ichi;
	register YesOrNo	*yes_no;
	register Inspect	*ins;
	extern int		screen;
	int 		x_cood, y_cood;
	XGCValues	xgcv;
	XSetWindowAttributes	attributes;
	extern	int	xerror(), xioerror();
	extern	char	*getenv();
	unsigned long pixel;
	register int	cnt;
	Cursor	cursor;

	XSizeHints	hints;

	if((dpy = XOpenDisplay(display_name)) == NULL)
	{
		fprintf(stderr,"Could not open Display : %s", display_name);
		return(-1);
	}
	if (ret = XInternAtom(dpy, "XJPFRONTEND", True)) {
		if (XGetSelectionOwner(dpy, ret)) {
			fprintf(stderr,"xwnmo already running");
			return(-1);
		}
	}
	if((xjc = (XJClient *)malloc(ret = sizeof(XJClient))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((jutilxjc = (XJClient *)malloc(ret = sizeof(XJClient))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	clear_xjc(xjc);
	clear_xjc(jutilxjc);
	if((ichi = (Ichiran *)malloc(ret = sizeof(Ichiran))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((ins = (Inspect *)malloc(ret = sizeof(Inspect))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((yes_no = (YesOrNo *)malloc(ret = sizeof(YesOrNo))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((jfew = (JFEWSTR *)malloc(ret = sizeof(JFEWSTR))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((xjc->buf0= (int *)malloc(sizeof(int) * (MHL0 * 2 + 1))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((xjc->buf= (int *)malloc(sizeof(int) * (maxchg * 2 + 1))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((buffer1 = (unsigned char *)malloc(maxchg * 4)) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	if((buffer2 = (unsigned char *)malloc(maxchg * 4)) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	jfew->xjclient = xjc;
	jfew->jutilxjc = jutilxjc;
	jfew->ichi = ichi;
	jfew->inspect = ins;
	jfew->yes_no = yes_no;
	jfew->j_c = NULL;
	jfew->sel_ret = -1;
	
	XSetErrorHandler(xerror);
	XSetIOErrorHandler(xioerror);

	screen = DefaultScreen(dpy);
	cursor1 = XCreateFontCursor(dpy, 52);
	cursor2 = XCreateFontCursor(dpy, 60);
	cursor3 = XCreateFontCursor(dpy, 24);

	if (*ascii_font == NULL)
		strncpy(ascii_font, DEFANKFONT, 256);
	if ((xjc->h_f = XLoadQueryFont(dpy, ascii_font)) == NULL) {
		fprintf(stderr, "Can't load font : %s\n", ascii_font);
		return(-1);
	}
	if (*ascii_escape == NULL)
		strncpy(xjc->h_escape, XJP_romanX0201, 4);
	else
		strncpy(xjc->h_escape, ascii_escape, 4);

	if (*kanji_font == NULL)
		strncpy(kanji_font, DEFKANJIFONT, 256);
	if ((xjc->k_f = XLoadQueryFont(dpy, kanji_font)) == NULL) {
		fprintf(stderr, "Can't load font : %s\n", kanji_font);
		return(-1);
	}
	if (*kanji_escape == NULL)
		strncpy(xjc->k_escape, XJP_kanjiX0208, 4);
	else
		strncpy(xjc->k_escape, kanji_escape, 4);

	if (*kana_font == NULL)
		strncpy(kana_font, DEFKANAFONT, 256);
	if ((xjc->r_f = XLoadQueryFont(dpy, kana_font)) == NULL) {
		fprintf(stderr, "Can't load font : %s\n", kana_font);
		return(-1);
	}
	if (*kana_escape == NULL)
		strncpy(xjc->r_escape, XJP_kanaX0201, 4);
	else
		strncpy(xjc->r_escape, kana_escape, 4);

	xjc->g_f = NULL;

	if (*util_ascii_font == NULL) {
		strncpy(util_ascii_font, ascii_font, 256);
		jutilxjc->h_f = xjc->h_f;
	} else if ((jutilxjc->h_f = XLoadQueryFont(dpy, util_ascii_font)) == NULL) {
		fprintf(stderr, "Can't load font : %s\n", util_ascii_font);
		return(-1);
	}

	if (*util_kanji_font == NULL) {
		strncpy(util_kanji_font, kanji_font, 256);
		jutilxjc->k_f = xjc->k_f;
	} else if ((jutilxjc->k_f = XLoadQueryFont(dpy, util_kanji_font)) == NULL) {
		fprintf(stderr, "Can't load font : %s\n", util_kanji_font);
		return(-1);
	}

	if (*util_kana_font == NULL) {
		strncpy(util_kana_font, kana_font, 256);
		jutilxjc->r_f = xjc->r_f;
	} else if ((jutilxjc->r_f = XLoadQueryFont(dpy, util_kana_font)) == NULL) {
		fprintf(stderr, "Can't load font : %s\n", util_kana_font);
		return(-1);
	}

	/* ウィンドウ作成 	*/
	if (fore_color[0] != NULL) {
		if ((pixel = Resolve_Color(RootWindow(dpy,screen), fore_color))
		    != 0xffffffff) {
			xjc->fg = pixel;
		} else {
			xjc->fg = BlackPixel(dpy, screen);
		}
	} else {
		xjc->fg = BlackPixel(dpy, screen);
	}
	if (back_color[0] != NULL) {
		if ((pixel = Resolve_Color(RootWindow(dpy,screen), back_color))
		    != 0xffffffff) {
			xjc->bg = pixel;
		} else {
			xjc->bg = WhitePixel(dpy, screen);
		}
	} else {
		xjc->bg = WhitePixel(dpy, screen);
	}
	if (boder_color[0] != NULL) {
		if ((pixel = Resolve_Color(RootWindow(dpy,screen), boder_color))
		    != 0xffffffff) {
			boderground = pixel;
		} else {
			boderground = xjc->fg;
		}
	} else {
		boderground = xjc->fg;
	}
	if (def_bwidth == 0)
		def_bwidth = 4;
	if (def_width == 0)
		def_width = DisplayWidth(dpy, screen);
	xjc->width = xjc->p_width = def_width - (def_bwidth * 2);
	xjc->height = FontHeight(xjc) + FontHeight(xjc)/2;
	xjc->columns = def_width/FontWidth(xjc);
	xjc->x = FontWidth(xjc) * (MHL0 + 1);
	xjc->y = (xjc->height - FontHeight(xjc))/ 2;
	xjc->max_columns = xjc->columns - (MHL0 + 2);
	xjc->vst = 0;
	xjc->note_fore = 0;
	xjc->note_back = 0;
	xjc->firstx = 0;
	xjc->firsty = xjc->y;
	xjc->currentcol = xjc->currentcol0 = 0;
	x_cood = 0;
	y_cood = DisplayHeight(dpy, screen) - xjc->height - (def_bwidth * 2);
	xjc->maxcolumns = maxchg * 2;
	maxlength = xjc->maxcolumns;

	xjc->wp = XCreateSimpleWindow(dpy,
			RootWindow(dpy, screen),
			x_cood, y_cood, xjc->width, xjc->height, def_bwidth,
			xjc->fg, xjc->bg);
	XSetWindowBorder(dpy, xjc->wp, boderground);
	XDefineCursor(dpy, xjc->wp, cursor3);
	XStoreName(dpy, xjc->wp, "Xwnmo");

	hints.flags = USSize | USPosition;
	hints.x = x_cood;
	hints.y = y_cood;
	hints.width = xjc->width;
	hints.height = xjc->height;
	XSetNormalHints(dpy, xjc->wp, &hints);

	xjc->wp1 = XCreateSimpleWindow(dpy, xjc->wp,
			xjc->x, xjc->y,
			xjc->max_columns * FontWidth(xjc),
			FontHeight(xjc), 0, xjc->fg, xjc->bg);

	xjc->w1 = XCreateSimpleWindow(dpy, xjc->wp1,
			0, 0,
			(xjc->maxcolumns + 1) * FontWidth(xjc),
			FontHeight(xjc), 0, xjc->fg, xjc->bg);

	xjc->wnf = XCreateSimpleWindow(dpy, xjc->wp,
			xjc->x - FontWidth(xjc), xjc->y,
			FontWidth(xjc),
			FontHeight(xjc), 0, xjc->fg, xjc->bg);

	xjc->wnb = XCreateSimpleWindow(dpy, xjc->wp,
			xjc->x + xjc->max_columns * FontWidth(xjc), xjc->y,
			FontWidth(xjc),
			FontHeight(xjc), 0, xjc->fg, xjc->bg);

	xjc->wnn = XCreateSimpleWindow(dpy, xjc->wp,
			xjc->x + (xjc->max_columns - 1)* FontWidth(xjc), xjc->y,
			FontWidth(xjc),
			FontHeight(xjc), 0, xjc->fg, xjc->bg);

	xjc->ws = XCreateSimpleWindow(dpy, xjc->wp,
			0, xjc->y, FontWidth(xjc) * MHL0, FontHeight(xjc), 0,
			xjc->fg, xjc->bg);
	XDefineCursor(dpy, xjc->ws, cursor2);

	xjc->wt = XCreateSimpleWindow(dpy, xjc->wp,
			0, 0, xjc->width, xjc->height, 0,
			xjc->fg, xjc->bg);

	attributes.override_redirect = True;
	attributes.save_under = True;
	attributes.backing_store = Always;
	XChangeWindowAttributes (dpy, xjc->wp,
			/*CWOverrideRedirect | */CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, xjc->w1,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, xjc->wnf,
			CWSaveUnder | CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, xjc->wnb,
			CWSaveUnder | CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, xjc->ws,
			CWSaveUnder | CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, xjc->wt,
			CWSaveUnder | CWBackingStore, &attributes);


    jutilxjc->fg = xjc->fg;
    jutilxjc->bg = xjc->bg;

    xgcv.foreground = xjc->fg;
    xgcv.background = xjc->bg;
    if (xjc->h_f != NULL){	/*      半角文字ＧＣ    */
	xjc->gch = XCreateGC(dpy, RootWindow(dpy, screen),
			     (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, xjc->gch, xjc->h_f->fid);
	jutilxjc->gch = XCreateGC(dpy, RootWindow(dpy, screen),
			     (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jutilxjc->gch, jutilxjc->h_f->fid);
    }
    if (xjc->k_f != NULL){	/*      全角文字ＧＣ    */
	xjc->gck = XCreateGC(dpy, RootWindow(dpy, screen),
			     (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, xjc->gck, xjc->k_f->fid);
	jutilxjc->gck = XCreateGC(dpy, RootWindow(dpy, screen),
			     (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jutilxjc->gck, jutilxjc->k_f->fid);
    }
    if (xjc->r_f != NULL){	/* 	kana文字ＧＣ	*/
	xjc->gcr = XCreateGC(dpy, RootWindow(dpy, screen),
			     (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, xjc->gcr, xjc->r_f->fid);
	jutilxjc->gcr = XCreateGC(dpy, RootWindow(dpy, screen),
			     (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jutilxjc->gcr, jutilxjc->r_f->fid);
    }
    xgcv.foreground = xjc->bg;
    xgcv.background = xjc->fg;
    if (xjc->h_f != NULL){	/*      半角文字ＧＣ    */
	xjc->reversegch = XCreateGC(dpy, RootWindow(dpy, screen),
			            (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, xjc->reversegch, xjc->h_f->fid);
	jutilxjc->reversegch = XCreateGC(dpy, RootWindow(dpy, screen),
			            (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jutilxjc->reversegch, jutilxjc->h_f->fid);
    }
    if (xjc->k_f != NULL){	/*      全角文字ＧＣ    */
	xjc->reversegck = XCreateGC(dpy, RootWindow(dpy, screen),
			            (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, xjc->reversegck, xjc->k_f->fid);
	jutilxjc->reversegck = XCreateGC(dpy, RootWindow(dpy, screen),
			            (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jutilxjc->reversegck, jutilxjc->k_f->fid);
    }
    if (xjc->r_f != NULL){	/* 	kana文字ＧＣ	*/
	xjc->reversegcr = XCreateGC(dpy, RootWindow(dpy, screen),
			            (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, xjc->reversegcr, xjc->r_f->fid);
	jutilxjc->reversegcr = XCreateGC(dpy, RootWindow(dpy, screen),
			            (GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jutilxjc->reversegcr, jutilxjc->r_f->fid);
    }

	dic_info->ascii_font_id = jutilxjc->h_f->fid;
	dic_info->kanji_font_id = jutilxjc->k_f->fid;
	dic_info->kana_font_id = jutilxjc->r_f->fid;
	dic_info->fore_ground = xjc->fg;
	dic_info->back_ground = xjc->bg;

	cur_x = jfew->xjclient;
	cur_p = jfew->xjclient;
	create_title();
	redraw_windowt(jfew->xjclient);
	create_ichi();
	create_yes_no();
	create_inspect();
#ifdef	DIRECT
	init_net();
#endif	/* DIRECT */
	write_jfewid();
/* MOTIF */
	read_motif_id();
/* MOTIF */
	XMapWindow(dpy, xjc->wp); 
	XMapWindow(dpy, xjc->ws); 

	XClearWindow(dpy, xjc->wp);

	{
	Window win;
	Pixmap pix;
	XWMHints hints;

	win = XCreateSimpleWindow(dpy, RootWindow(dpy,screen),0, 0,
				64,64,1, 1, 0);
	pix = XCreatePixmapFromBitmapData(dpy, xjc->wp, ic_bits,
				ic_width, ic_height, xjc->fg, xjc->bg,
				(unsigned int)DefaultDepth(dpy, screen));
	XSetWindowBackgroundPixmap(dpy, win, pix);
	hints.icon_pixmap = pix;
	hints.icon_window = win;
	hints.flags = IconPixmapHint|IconWindowHint;
	if (iconic_start) {
		hints.initial_state = IconicState;
		hints.flags |= StateHint;
	}
	XSetWMHints(dpy, xjc->wp, &hints);
	XFlush(dpy);
	}

	XSelectInput(dpy, xjc->wp,
		KeyPressMask | ExposureMask | ButtonPressMask 
		| ButtonReleaseMask | ResizeRedirectMask | StructureNotifyMask);
	XSelectInput(dpy, xjc->w1, ExposureMask);
	XSelectInput(dpy, xjc->wt, ExposureMask);
	XSelectInput(dpy, xjc->wnf, ExposureMask);
	XSelectInput(dpy, xjc->wnb, ExposureMask);
	XSelectInput(dpy, xjc->ws, 
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XFlush(dpy);
	client_mem[0] = xjc;

	fflush(stdin);

}
clear_xjc(x)
XJClient *x;
{
	x->max_l1 = 0;
	x->max_l2 = 0;
	x->max_l3 = 0;
	x->max_pos = 0;
	x->visible = 0;
	x->visible_line = 0;
	x->max_cur = 0;
	x->cursor_flag = 0;
	x->n_write = 0;
	x->mark_flag = 0;
}

create_title()
{
	extern char	*romkan_offmode();
	char	*p;

	if (*xwnmo_title == NULL)
		strcpy(xwnmo_title, XWNMO_TITLE);
	strcat(xwnmo_title, ((p = romkan_offmode()) == NULL ? "[---]" : p));
}

create_ichi() 
{
	XSetWindowAttributes	attributes;
	XGCValues	xgcv;
	Ichiran		*ichi = jfew->ichi;
	XJClient	*xjc = jfew->jutilxjc;
	extern int		screen;
	unsigned int	width, height, bwidth;
	int	cnt;
	struct wnmo_client  *save_c_c;

	screen = DefaultScreen(dpy);

	width = jfew->xjclient->width;
	height = jfew->xjclient->height;
	bwidth = 2;

	xgcv.foreground = xjc->fg;
	xgcv.function = GXinvert;
	xgcv.plane_mask = XOR(xjc->fg, xjc->bg);
	ichi->invertgc = XCreateGC(dpy, RootWindow(dpy, screen),
				GCForeground | GCFunction | GCPlaneMask,&xgcv);

	ichi->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
			width, height, bwidth, xjc->fg, xjc->bg);
	ichi->w1 = XCreateSimpleWindow(dpy, ichi->w, 0, FontWidth(xjc) * 4,
			width, height, 1, xjc->fg, xjc->bg);
	ichi->title_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), 1, xjc->fg, xjc->bg);
	ichi->cancel_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), bwidth, xjc->fg, xjc->bg);
	ichi->mouse_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), bwidth, xjc->fg, xjc->bg);
	ichi->subtitle_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), 1, xjc->fg, xjc->bg);
	ichi->exec_title_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), bwidth, xjc->fg, xjc->bg);
	ichi->next_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), bwidth, xjc->fg, xjc->bg);
	ichi->back_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc), FontHeight(xjc), bwidth, xjc->fg, xjc->bg);
	ichi->nyuu_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth(xjc) * 5, FontHeight(xjc), 1, xjc->fg, xjc->bg);
			
	XDefineCursor(dpy, ichi->w, cursor3);
	XDefineCursor(dpy, ichi->cancel_w, cursor2);
	XDefineCursor(dpy, ichi->mouse_w, cursor2);
	XDefineCursor(dpy, ichi->exec_title_w, cursor2);
	XStoreName(dpy, ichi->w, "ichiran");
	XSelectInput(dpy, ichi->w, KeyPressMask | StructureNotifyMask);
	XSelectInput(dpy, ichi->title_w, ExposureMask);
	XSelectInput(dpy, ichi->cancel_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ichi->mouse_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ichi->exec_title_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ichi->next_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ichi->back_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);

/*
	attributes.override_redirect = True;
	XChangeWindowAttributes (dpy, ichi->w, CWOverrideRedirect, &attributes);
*/

	attributes.backing_store = Always;
	XChangeWindowAttributes (dpy, ichi->title_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->cancel_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->mouse_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->w1,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->subtitle_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->exec_title_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->next_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->back_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->nyuu_w,
			CWBackingStore, &attributes);
	XMapWindow(dpy, ichi->title_w);
	XMapWindow(dpy, ichi->cancel_w);
	XFlush(dpy);
}

create_yes_no()
{
    XSetWindowAttributes	attributes;
    XJClient	*xjc = jfew->jutilxjc;
    YesOrNo *yes_no = jfew->yes_no;
    extern int		screen;
    unsigned int	width, height, bwidth;
    int	cnt;

    screen = DefaultScreen(dpy);

    width = 1;
    height = 1;
    bwidth = 2;

    yes_no->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
			width, height, bwidth, xjc->fg, xjc->bg);
    yes_no->title_w = XCreateSimpleWindow(dpy, yes_no->w, 0,0,
			width, height, 0, xjc->fg, xjc->bg);
    yes_no->yes_w = XCreateSimpleWindow(dpy, yes_no->w, FontWidth(xjc) * 2,
			FontHeight(xjc) * 2 + FontHeight(xjc)/2 - IN_BORDER,
			FontWidth(xjc) * 6 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, bwidth,
			xjc->fg, xjc->bg);
    yes_no->no_w = XCreateSimpleWindow(dpy, yes_no->w, FontWidth(xjc) * 10,
			FontHeight(xjc) * 2 + FontHeight(xjc)/2 - IN_BORDER,
			FontWidth(xjc) * 6 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, bwidth,
			xjc->fg, xjc->bg);

    XDefineCursor(dpy, yes_no->w, cursor3);
    XDefineCursor(dpy, yes_no->yes_w, cursor2);
    XDefineCursor(dpy, yes_no->no_w, cursor2);
    XSelectInput(dpy, yes_no->yes_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
    XSelectInput(dpy, yes_no->no_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
    XSelectInput(dpy, yes_no->title_w, ExposureMask);

    attributes.override_redirect = True;
    attributes.backing_store = Always;
    XChangeWindowAttributes (dpy, yes_no->w, 
			CWOverrideRedirect|CWBackingStore, &attributes);
    XChangeWindowAttributes (dpy, yes_no->title_w,
			CWBackingStore, &attributes);
    XChangeWindowAttributes (dpy, yes_no->yes_w,
			CWBackingStore, &attributes);
    XChangeWindowAttributes (dpy, yes_no->no_w,
			CWBackingStore, &attributes);
    XMapWindow(dpy, yes_no->title_w);
    XFlush(dpy);
}

create_inspect()
{
	XSetWindowAttributes	attributes;
	Inspect		*ins = jfew->inspect;
	XJClient	*xjc = jfew->jutilxjc;
	extern int		screen;
	unsigned int	width, height, bwidth;
	int	cnt;
	struct wnmo_client  *save_c_c;

	width = jfew->xjclient->width;
	height = jfew->xjclient->height;
	bwidth = 2;

	ins->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
			width, height, bwidth, xjc->fg, xjc->bg);
	ins->w1 = XCreateSimpleWindow(dpy, ins->w, 0, FontHeight(xjc) * 2,
			width, height, 1, xjc->fg, xjc->bg);
	ins->title_w = XCreateSimpleWindow(dpy, ins->w,
			FontWidth(xjc), FontHeight(xjc)/2,
			FontWidth(xjc) * 12 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, 1, xjc->fg, xjc->bg);
	ins->cancel_w = XCreateSimpleWindow(dpy, ins->w,
			FontWidth(xjc) * 16, FontHeight(xjc)/2,
			FontWidth(xjc) * 10 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, 1, xjc->fg, xjc->bg);
	ins->del_w = XCreateSimpleWindow(dpy, ins->w,
			FontWidth(xjc) * 28, FontHeight(xjc)/2,
			FontWidth(xjc) * 4 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, 1, xjc->fg, xjc->bg);
	ins->use_w = XCreateSimpleWindow(dpy, ins->w,
			FontWidth(xjc) * 34, FontHeight(xjc)/2,
			FontWidth(xjc) * 11 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, 1, xjc->fg, xjc->bg);
	ins->next_w = XCreateSimpleWindow(dpy, ins->w,
			FontWidth(xjc) * 48, FontHeight(xjc)/2,
			FontWidth(xjc) * 4 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, 1, xjc->fg, xjc->bg);
	ins->back_w = XCreateSimpleWindow(dpy, ins->w,
			FontWidth(xjc) * 54, FontHeight(xjc)/2,
			FontWidth(xjc) * 4 + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2, 1, xjc->fg, xjc->bg);
			
	XDefineCursor(dpy, ins->w, cursor3);
	XDefineCursor(dpy, ins->cancel_w, cursor2);
	XDefineCursor(dpy, ins->use_w, cursor2);
	XDefineCursor(dpy, ins->del_w, cursor2);
	XDefineCursor(dpy, ins->next_w, cursor2);
	XDefineCursor(dpy, ins->back_w, cursor2);
	XStoreName(dpy, ins->w, "Inspect");
	XSelectInput(dpy, ins->w, KeyPressMask | StructureNotifyMask);
	XSelectInput(dpy, ins->title_w, ExposureMask);
	XSelectInput(dpy, ins->cancel_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ins->del_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ins->use_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ins->next_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);
	XSelectInput(dpy, ins->back_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);

/*
	attributes.override_redirect = True;
	XChangeWindowAttributes (dpy, ins->w, CWOverrideRedirect, &attributes);
*/

	attributes.backing_store = Always;
	XChangeWindowAttributes (dpy, ins->w1,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ins->title_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ins->cancel_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ins->del_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ins->use_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ins->next_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ins->back_w,
			CWBackingStore, &attributes);
	XMapSubwindows(dpy, ins->w);
	XFlush(dpy);
}

Resolve_Color(w, name)
Window	w;
char	*name;
{
	XColor c;
	Colormap colormap;
	XWindowAttributes wind_info;

	/*
	 * The following is a hack to insure machines without a rgb table
	 * handle at least white & black right.
	 */
	if (!strcmp(name, "white"))
	  name="#ffffffffffff";
	if (!strcmp(name, "black"))
	  name="#000000000000";

	XGetWindowAttributes(dpy, w, &wind_info);
	colormap = wind_info.colormap;

	if (!XParseColor(dpy, colormap, name, &c)) {
		fprintf(stderr,"Bad color format '%s', use default color\n", name);
		return(-1);
	}
	if (!XAllocColor(dpy, colormap, &c)) {
		fprintf(stderr,"XAllocColor failed! , use default color\n");
		return(-1);
	}
	return(c.pixel);
}

/*********************************************************************
	Ｐｒｏｐｅｒｔｙの登録 ルーチン
			from create_jfew()
*********************************************************************/

#define MAXLEN	128
#define VERSION	"\0\0\0\1"
#define FENAME 	"xwnmo.V2"
write_jfewid()
{
	unsigned char pro_name[16];
	register XJClient	*xjc;
	register int	cnt;
	char	buf[MAXLEN*2+10];
	char	name[MAXLEN];
	char	hname[MAXLEN];
	extern int port;
	long	pnumber;

	xjc = jfew->xjclient;

	/* JP_frontend ＩＤ用Property の登録 */
	if (!(select_id = XInternAtom(dpy, "XJPFRONTEND", True))) {
		select_id = XInternAtom(dpy, "XJPFRONTEND", False);
	}
	pnumber = (long)port;
	gethostname(hname, MAXLEN);
	strcpy(name, FENAME);
	bcopy(name, buf, MAXLEN);
	bcopy(hname, buf + MAXLEN, MAXLEN);
	bcopy(&pnumber, buf + MAXLEN * 2, sizeof(long));
	buf[MAXLEN * 2 + sizeof(long)] = 0;

	XChangeProperty(dpy, DefaultRootWindow(dpy), select_id, XA_STRING, 8,
          PropModeReplace, buf, MAXLEN * 2 + sizeof(long));

	XSetSelectionOwner(dpy, select_id, xjc->wp,  0L);
}

read_motif_id()
{
    register XJClient	*xjc;
    register int	cnt;

    xjc = jfew->xjclient;

    if ((motif_id = XInternAtom(dpy, "WM_PROTOCOLS", True))) {
	motif_id1 = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
	XChangeProperty(dpy, jfew->xjclient->wp, motif_id, XA_INTEGER, 32,
			PropModeReplace, &motif_id1, sizeof(Atom));
	XChangeProperty(dpy, jfew->ichi->w, motif_id, XA_INTEGER, 32,
			PropModeReplace, &motif_id1, sizeof(Atom));
	XChangeProperty(dpy, jfew->inspect->w, motif_id, XA_INTEGER, 32,
			PropModeReplace, &motif_id1, sizeof(Atom));
    } else {
	motif_id = motif_id1 = 0;
    }
}
/*********************************************************************
	終了 ルーチン
			from err_end(), do_end()
*********************************************************************/
xw_end()
{
	register int	cnt;
	XEvent		event;
	int		ret;

	if (dpy) {
	event.type = ClientMessage;
	event.xclient.format = 32;
	event.xclient.message_type = select_id;
	event.xclient.data.l[0] = XJP_ERROR;
	event.xclient.data.l[1] = 0;
	for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if (client_mem[cnt] != NULL) {
			destroy_jw(client_mem[cnt]);
#ifdef	DIRECT
			if (client_mem[cnt]->fd == -1) {
#endif	/* !DIRECT */
				XSendEvent(dpy, client_mem[cnt]->w, False, NoEventMask, &event);
				XFlush(dpy);
#ifdef	DIRECT
	  		}
#endif	/* !DIRECT */
		}
	}
	ret = XDeleteProperty(dpy, jfew->xjclient->wp, select_id);
	XDestroyWindow(dpy, jfew->xjclient->wnf);
	XDestroyWindow(dpy, jfew->xjclient->wnb);
	XDestroyWindow(dpy, jfew->xjclient->w1);
	XSync(dpy);
	free(jfew->xjclient);
	free(jfew);
#ifdef	DIRECT
	close_net();
#endif
	sleep(1);
	shutdown(dpy->fd, 2);
	close(dpy->fd);
	}
}

/*********************************************************************
	エラー発生時処理 ルーチン
			from  XSetErrorHandler()
*********************************************************************/
xerror(d, ev)
Display	*d;
register XErrorEvent *ev;
{
/*
	char	buffer[BUFSIZE];
	XGetErrorText(d, ev->error_code, buffer, BUFSIZE);
	fprintf(stderr, "xwnmo: %s\n", buffer);
	fprintf(stderr, "Request code %d, minor code %d, serial #%ld, resource id %ld\n",
		ev->request_code, ev->minor_code, ev->serial, (long)ev->resourceid);
	do_end();
*/
}

/*********************************************************************
	ＩＯエラー発生時処理 ルーチン
			from XSetIOErrorHandler()
*********************************************************************/
xioerror(d)
Display	d;
{
	perror("xwnmo");
	epilogue();
#ifdef	DIRECT
	close_net();
#endif
	shutdown(dpy->fd, 2);
	close(dpy->fd);
	exit(2);
}

/*********************************************************************
	イベント処理 ルーチン
			from keyin0()
*********************************************************************/

static	XComposeStatus compose_status = {NULL, 0};

xw_read(buff)
register unsigned char	*buff;
{
	XEvent event;
	int		code;
	char		strbuf[STRBUFSIZE];	
	register int	ret;
	int		keycode;
	int		nbytes;
	int		ck;

	if (cur_x->dispmode == 1 && cur_p->del_x >= cur_p->max_pos) {
		JWMline_clear1();
	}

	for(;;){
#ifdef	DIRECT
	XFlush(dpy);
	if (cur_x != NULL && cur_x->fd != -1 && cur_x->n_write) {
		return_socket_null();
	}
	LXNextEvent(dpy, &event);
#else	/* !DIRECT */
	XNextEvent(dpy, &event);
#endif	/* !DIRECT */
	switch (event.type) {
		
		case KeyPress:
			if (cur_input == 0) {
				if (cur_cl_set(event.xkey.window) != 0) {
					break;
				}
			} else if (check_cur_input(event.xkey.window)) {
				if(cur_cl_change(event.xkey.window) != 0) {
					break;
				}
			}
			
			nbytes = XLookupString (&event, strbuf, STRBUFSIZE,
						&keycode, &compose_status);

#ifdef	DIRECT
			if (cur_x->n_write) {
				save_sockbuf(ret_sockbuf, nbytes, keycode,
					strbuf);
			}
#endif	/* DIRECT */
			if (cur_input)
				cur_input->save_event = 0;
			if (event.xkey.state & Mod1Mask) { /* 全面キー */
				if ((ck = cvtmeta(keycode)) == -1) {
					if (jumpkey_mode)
						send_cl_key(&event.xkey);
					break;
				} else {
					if (henkan_off_flag)
						save_event(event.xkey);
					*buff++ = ck;
					return(1);
				}
			}

			if ((keycode & 0xff00) == 0xff00) { 
				/* ファンクションキー */
				if ((keycode >= XK_Multi_key && keycode <= XK_Eisuu_Lock) ||
				(keycode >= XK_Home && keycode <= XK_F35)) {
					if ((ck = cvtfun(keycode)) == -1) {
						if (jumpkey_mode)
							send_cl_key(&event.xkey);
						break;
					} else {
						if (henkan_off_flag)
							save_event(event.xkey);
						*buff++ = ck;
						return(1);
					}
				} else {
					if (nbytes > 0) {
						if (ifempty() || henkan_off_flag)
							save_event(event.xkey);
						strncpy(buff, strbuf, nbytes);
						buff += nbytes;
						return(nbytes);
					} else {
						break;
					}
				}
			}

			if (event.xkey.state & ControlMask) {
				/* コントロールキー */
				if (*strbuf >= 0x20) {
					if (ifempty() || henkan_off_flag)
						send_cl_key(&event.xkey);
					break;
				} else if (*strbuf == 0x0) {
					if (henkan_off_flag)
						save_event(event.xkey);
					*buff++ = *strbuf;
					return(1);
				} else if (*strbuf <= 0x1f) {
					if (ifempty() || henkan_off_flag)
						save_event(event.xkey);
					*buff++ = *strbuf;
					return(1);
				}
			}
			
			if (( keycode >= 0x4a0) && (keycode <= 0x4df)){
				/* 仮名キー */
				if (henkan_off_flag)
					save_event(event.xkey);
				*buff++ = SS2;
#ifdef	X11R3
				*buff = (char)(keycode & 0xff);
				return(2);
#else	/* X11R3 */
				strncpy(buff, strbuf, nbytes);
				return(nbytes + 1);
#endif	/* X11R3 */
			} else if ((keycode < 0x100) && (keycode > 0)){
				if (henkan_off_flag)
					save_event(event.xkey);
				strncpy(buff, strbuf, nbytes);
				return(nbytes);
			}
			break;

		case ClientMessage: /* 日本語ツールキットからの要求	*/
/* MOTIF */
			if (motif_id && event.xclient.message_type == motif_id
			    && event.xclient.data.l[0] == motif_id1) {
				if (motif_event(&event) == 0) {
					*buff = 0xd;
					return(1);
				}
				break;
			}
/* MOTIF */
			switch(event.xclient.data.l[0]) {

				case XJP_OPEN:
					if (cl_touroku(event.xclient.data.l[2],
							event.xclient.data.l[1]))
						fprintf(stderr,"touroku error\n");
					break;

				case XJP_CLOSE:
					cl_sakujo(event.xclient.window);
					break;

				case XJP_BEGIN:
					if(key_grab(event.xclient.data.l[2],
						event.xclient.window) == -1)
						fprintf(stderr,"grab error");
					break;

				case XJP_END:
						key_ungrab(event.xclient.data.l[2],
							event.xclient.window);
					break;

				case XJP_VISIBLE:
					map_window3(event.xclient.window);
					break;

				case XJP_INVISIBLE:
					unmap_window3(event.xclient.window);
					break;

				case XJP_CHANGE:
					/* なんでもできる変更ルーチンを呼ぼう */
					cl_change(event.xclient.data.l[1]);
					break;

				case XJP_MOVE:
					cl_move(event.xclient.data.l[1],
						event.xclient.data.l[2],
						event.xclient.data.l[3]);
					break;

				case DIC_START:
					dicserver_id 
					    = (Window)event.xclient.data.l[1];
					break;

				default:
					break;
			}
			break;

		case Expose:
			xw_expose(&event);
			break;
		case ButtonPress:
			button_x = event.xbutton.x_root;
			button_y = event.xbutton.y_root;
			if (event.xbutton.window == jfew->xjclient->wp) {
				select_button = 1;
				syuuryou_menu();
			} else {
			if(xw_buttonpress(&event) == 0) {
			  if (syuuryou &&
			  (event.xbutton.window == jfew->ichi->cancel_w ||
			  event.xbutton.window == jfew->ichi->w1)) {
				syuuryou_menu();
			  } else {
				*buff = 0xd;
				return(1);
			  }
			}
			}
			break;
		case DestroyNotify:
			cl_destroy(event.xdestroywindow.window);
			break;
		case MotionNotify:
			xw_mousemove(&event);
			break;
		case EnterNotify:
			xw_enterleave(&event, 1);
			break;
		case LeaveNotify:
			xw_enterleave(&event, 0);
			break;
		case ReparentNotify:
			read_motif_id();
			break;
		default:
			break;
		
	}
	}
}

motif_event(ev)
XClientMessageEvent *ev;
{
	if (ev->window == jfew->xjclient->wp) {
		kill(getpid(), SIGTERM);
	} else if (ev->window == jfew->ichi->w
		   || ev->window == jfew->inspect->w) {
		select_return = -2;
		cur_cl_change3(jfew->j_c);
		return(0);
	}
    	return(1);
}

check_cur_input(w)
Window w;
{
	if (cur_input->pclient->dispmode == 1) {
		if (w != cur_input->pclient->w &&
		w != cur_input->pclient->wp1 &&
		w != cur_input->pclient->wp2 &&
		w != cur_input->pclient->wp3 &&
		w != cur_input->pclient->wnf &&
		w != cur_input->pclient->wnb &&
		w != cur_input->pclient->ws) {
			return(1);
		}
	} else {
		if (w != cur_input->pclient->w &&
		w != cur_input->pclient->wp)
			return(1);
	}
	return(0);
}

static	char	*syuuryou_title = "漢字変換(XWNMO)";
static	char	*syuuryou_syuuryou = "   処理終了";
syuuryou_menu()
{
	static char *buf[1];
	int	c;

	cur_x = jfew->xjclient;
	cur_p = cur_x;
	c_c = cur_p->w_c;
	cur_rk = c_c->rk;
	cur_input = 0;
	syuuryou = 1;
	buf[0] = syuuryou_syuuryou;
	c = xw_select_one_element(buf, 1, -1,syuuryou_title, SENTAKU,
				main_table[4]);
	if (c == 0) {
		kill(getpid(),SIGTERM);
	} else if (c == BUFFER_IN_CONT) {
		return(BUFFER_IN_CONT);
	}
	syuuryou = 0;
	return(0);
}

/*********************************************************************
	イベント処理 ルーチン
		from do_main(), xw_write_char()
*********************************************************************/
xw_write(buff, size)
register wchar	*buff;
register int	size;
{
	
	register unsigned char	*str1;
	register unsigned char	*str2;
	register unsigned char	*save_buf;
	register int	wc;
	XEvent 		event;
	int		nsize;
	register int	cnt;
	int		code;
	static int	sel_cont = 0;
	int		char2 = 0;
	

	if (henkan_off_flag) {
		return_cl_it();
		return;
	}
	str1 = buffer1;
	str2 = buffer2;

	if (!size) return;

	if (cur_x->dispmode == 1 && cur_p->del_x >= cur_p->max_pos) {
		JWMline_clear1();
	}

	/* wchar を char に変換 */
	for (cnt = 0; cnt < size; cnt++) {
		wc = *buff++;
		if (wc <= 0xff) {
			if ((wc >= 0xa1) && (wc <= 0xdf))
				*str1++ = SS2;
			*str1++ = wc;
		} else {
			*str1++ = (wc >> 8) & 0xff;
			*str1++ = wc & 0xff;
		}
	}
	*str1 = '\0';
	nsize = str1 - buffer1;
	str1 = buffer1;

#ifdef	DIRECT
	if (cur_x->fd != -1) {
		return_socket(str1);
	} else
#endif	/* DIRECT */
	{	/* UJIS -> JIS */
		save_buf = str2;
		while (*str1) {
			if (((unsigned char)*str1 >= 0xa1 && (unsigned char)*str1 <= 0xfe)) {
				if (char2 != 1 && (save_buf != str2)) {
					*str2 = '\0';
					send_buf(save_buf, char2);
					save_buf = str2;
				}
				*str2++ = *str1++ & 0x7f;
				*str2++ = *str1++ & 0x7f;
				char2 = 1;
			} else if ((unsigned char)*str1 == SS2) {
				if (char2 != 2 && (save_buf != str2)) {
					*str2 = '\0';
					send_buf(save_buf, char2);
					save_buf = str2;
				}
				++str1;
				*str2++ = *str1++ & 0x7f;
				char2 = 2;
			} else if ((unsigned char)*str1 == SS3) {
				if (char2 != 3 && (save_buf != str2)) {
					*str2 = '\0';
					send_buf(save_buf, char2);
					save_buf = str2;
				}
				++str1;
				*str2++ = *str1++ & 0x7f;
				*str2++ = *str1++ & 0x7f;
				char2 = 3;
			} else {
				if (char2 != 0 && (save_buf != str2)) {
					*str2 = '\0';
					send_buf(save_buf, char2);
					save_buf = str2;
				}
				*str2++ = *str1++;
				char2 = 0;
			}
		}
		*str2 = '\0';
		send_buf(save_buf, char2);
	}
}
send_buf(str ,flg)
register char	*str;
int		flg;
{
	XEvent 		event;
	register int	cnt;
	register int	total;
	
	total = euclen(str);
	event.type = ClientMessage;
	event.xclient.format = 8;
	event.xclient.message_type = select_id;
	event.xclient.window = cur_input->w;
	if (flg == 3) {
		bcopy(cur_p->g_escape, event.xclient.data.b, 2);
	} else if (flg == 2) {
		bcopy(cur_p->r_escape, event.xclient.data.b, 2);
	} else if (flg == 1) {
		bcopy(cur_p->k_escape, event.xclient.data.b, 2);
	} else {
		bcopy(cur_p->h_escape, event.xclient.data.b, 2);
	}
	while(*str) {
		for (cnt = 4; cnt < 4 + total && cnt < 20 ; cnt++ ) {
			event.xclient.data.b[cnt] = *str++;
		}
		event.xclient.data.b[3] = (char)(cnt - 4);
		XSendEvent(dpy, cur_input->w, False, NoEventMask, &event);
		total = total - 16;
	}
}

#ifdef	DIRECT
return_socket_null()
{
	bzero(ret_sockbuf, 8);
	cur_x->n_write = 0;
	write(cur_x->fd, ret_sockbuf, 8);
}
return_socket(str)
unsigned char	*str;
{
	short	len;

	if (str == NULL) {
		bcopy(ret_sockbuf + 2, &len, 2);
	} else {
		len = strlen(str);
		save_sockbuf(ret_sockbuf, len, (int)0, str);
	}
	cur_x->n_write = 0;
	write(cur_x->fd, ret_sockbuf, len + 8);
}

return_socket_ok(fd)
int fd;
{
    bzero(ret_sockbuf, 4);
    write(fd, ret_sockbuf, 4);
}

return_socket_error(fd, no)
int fd;
int no;
{
    make_err_socket(ret_sockbuf, no);
    write(fd, ret_sockbuf, 4);
}

#endif	/* DIRECT */

key_grab(get_w, w)
Window	get_w;
Window	w;
{
	register int	cnt;
	register int	cntt;
	XJClient *save_cur_p, *save_cur_x;

	if (get_w == w)
		XSelectInput(dpy, get_w, KeyPressMask | StructureNotifyMask);
	else
		XSelectInput(dpy, get_w, KeyPressMask);
	XFlush(dpy);
	for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if (client_mem[cnt] != NULL && client_mem[cnt]->w == w) {
			for(cntt = 0; cntt < input_num; cntt++) {
				if (input_mem[cntt]->w == get_w) {
					return(0);
				}
			}
			if((input_mem[input_num] = (XJInput *)malloc(ret = sizeof(XJInput))) == NULL){
				fprintf(stderr, "error malloc\n");
				return(-1);
			}
			input_mem[input_num]->w = get_w;
			input_mem[input_num]->pclient = client_mem[cnt];
			input_num++;
			return(0);
		}
	}
	return(-1);
		
}

/********************************************************************
	キーイベント開放 ルーチン
			from xw_read(), henkan_off()
********************************************************************/
key_ungrab(unget_w, w)
Window	unget_w;
Window	w;
{
	register int	cnt;
	XJClient *save_cur_p, *save_cur_x;

	if (unget_w == w)
		XSelectInput(dpy, unget_w, StructureNotifyMask);
	else
		XSelectInput(dpy, unget_w, NoEventMask);
	XFlush(dpy);
	return(0);
}

ifempty()
{
	if (cur_p->max_pos == 0) {
		return(1);
	} else {
		return(0);
	}
}

send_err(w, no)
Window w;
int no;
{
	XEvent	event;

	event.type = ClientMessage;
	event.xclient.format = 32;
	event.xclient.window = jfew->xjclient->w;
	event.xclient.data.l[0] = XJP_ERROR;
	event.xclient.data.l[1] = no;
	XSendEvent(dpy, w, False, NoEventMask, &event);
	XFlush(dpy);
	return(0);
}

send_cl_key(event)
XKeyEvent	*event;
{
#ifdef	DIRECT
	if (cur_x->fd != -1) {
		return_socket(NULL);
	} else
#endif
	if (cur_input) {
		event->window = cur_input->w;
		XSendEvent(dpy, cur_input->w, False, NoEventMask, event);
	}
	XFlush(dpy);
}

save_event(event)
XKeyEvent	event;
{
	if (cur_input) {
		event.window = cur_input->w;
		bcopy(&event, &(cur_input->event), sizeof(XKeyEvent));
		cur_input->save_event = 1;
	}
}

return_cl_it()
{
#ifdef	DIRECT
	if (cur_x != 0 && cur_x->fd != -1) {
		return_socket(NULL);
	} else
#endif
	if (cur_input != 0 && cur_input->save_event) {
		send_cl_key(&(cur_input->event));
		cur_input->save_event = 0;
	}
}
	
/**********************************************************************
	クライアント登録 ルーチン
			from xw_read()
**********************************************************************/
cl_touroku(cl_atom, win)
Atom	cl_atom;	/* クライアントの情報が格納されているPropertyのAtom番号 */
Window	win;		/* クライアントのウィンドウＩＤ */
{
	register XJClient	*xjc;
	XGCValues	xgcv;
	Atom		actual_type;
	int		actual_format;
	unsigned long	nitems;
	unsigned long	byte_after;
	unsigned long	leftover;
	unsigned char	*data;
	XEvent		event;
	int		ank = 0;
	int		kanji = 0;
	int		kana = 0;
	int		gaiji = 0;
	register int	cnt;
	struct wnmo_client *save_c_c, *old_c_c;
	XJClient	*save_cur_p, *save_cur_x;
	char		save_escape[4];
	char		save_font[128];

	if((xjc = (XJClient *)malloc(ret = sizeof(XJClient))) == NULL){
		fprintf(stderr, "error in malloc(cl_touroku)\n");
		return(-1);
	}
	XGetWindowProperty(dpy, win, cl_atom, 0L, 1000000L, False, XA_STRING, 
			&actual_type, &actual_format, &nitems, &leftover, &data); 
	if (nitems < 184) {
		fprintf(stderr, "Bad Property %d\n", cl_atom);
		free(xjc);
		return(-1);
	}
	bcopy(data, xjc, 184);
#ifdef UX386
/*	サーバから受けたデータの内、long のもののバイト順を変更する	*/
	xjc->dispmode = htonl(xjc->dispmode);
	xjc->w = 	htonl(xjc->w);	
	xjc->mask = 	htonl(xjc->mask);
	xjc->p_width = 	htonl(xjc->p_width);
	xjc->p_height = htonl(xjc->p_height);
	xjc->x = 	htonl(xjc->x);
	xjc->y = 	htonl(xjc->y);
	xjc->width = 	htonl(xjc->width);
	xjc->height = 	htonl(xjc->height);
	xjc->fg = 	htonl(xjc->fg);
	xjc->bg =	htonl(xjc->bg);
	xjc->bp = 	htonl(xjc->bp);
	xjc->status_x = htonl(xjc->status_x);
	xjc->status_y = htonl(xjc->status_y);
#endif

	for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if (client_mem[cnt] != NULL && client_mem[cnt]->w == xjc->w) {
			client_mem[cnt]->re_count++;
			if (xjc->dispmode == 2)
				send_open(xjc, jfew->xjclient->wp);
			else
				send_open(xjc, client_mem[cnt]->w1);
			free(xjc);
			return(0);
		}
	}
	if (xjc->dispmode != 2) {
	    for(data = data + 184; *data;) {
		for (cnt = 0;*data != 0x09; cnt++, data++) {
			save_escape[cnt] = *data;
		}
		save_escape[cnt] = NULL;
		data++;
		for (cnt = 0;*data != 0x0a && *data; cnt++, data++) {
			save_font[cnt] = *data;
		}
		save_font[cnt] = NULL;
		data++;
		if (save_escape[0] == '(') {
			if (save_escape[1] == 'I') {
				strcpy(xjc->r_escape, save_escape);
				strcpy(xjc->r_font, save_font);
				kana = 1;
			} else {
				strcpy(xjc->h_escape, save_escape);
				strcpy(xjc->h_font, save_font);
				ank = 1;
			}
		} else if (save_escape[0] == '$') {
			if (save_escape[1] == 'B') {
				strcpy(xjc->k_escape, save_escape);
				strcpy(xjc->k_font, save_font);
				kanji = 1;
			} else {
				strcpy(xjc->g_escape, save_escape);
				strcpy(xjc->g_font, save_font);
				gaiji = 1;
			}
		}

	    }
	    /*	半角文字ＧＣ	*/
	    if ((xjc->mask & XJP_FG) == 0) xjc->fg = jfew->xjclient->fg;
	    if ((xjc->mask & XJP_BG) == 0) xjc->bg = jfew->xjclient->bg;
	    if(ank) {
		if((xjc->h_f = XLoadQueryFont(dpy, xjc->h_font)) == NULL)
			if((xjc->h_f = XLoadQueryFont(dpy, xjc->h_font)) == NULL) {
				fprintf(stderr, "Could not open font %s\n", xjc->h_font);
				send_err(xjc->w, XJP_F_OPEN_BAD_FN);
				free(xjc);
				return(-1);
			}
		xgcv.foreground = xjc->fg;
		xgcv.background = xjc->bg;
		xjc->gch = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->gch, xjc->h_f->fid);
		xgcv.foreground = xjc->bg;
		xgcv.background = xjc->fg;
		xjc->reversegch = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->reversegch, xjc->h_f->fid);
	    } else {
		send_err(xjc->w, XJP_F_OPEN_BAD_FN);
		free(xjc);
		return(-1);
	    }
	    /*	全角文字ＧＣ	*/
	    if(kanji) {
		if((xjc->k_f = XLoadQueryFont(dpy, xjc->k_font)) == NULL)
			if((xjc->k_f = XLoadQueryFont(dpy, xjc->k_font)) == NULL) {
				fprintf(stderr, "Could not open font %s\n", xjc->k_font);
				send_err(xjc->w, XJP_F_OPEN_BAD_FN);
				free(xjc);
				return(-1);
			}
		xgcv.foreground = xjc->fg;
		xgcv.background = xjc->bg;
		xjc->gck = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->gck, xjc->k_f->fid);
		xgcv.foreground = xjc->bg;
		xgcv.background = xjc->fg;
		xjc->reversegck = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->reversegck, xjc->k_f->fid);
	    } else {
		send_err(xjc->w, XJP_F_OPEN_BAD_FN);
		free(xjc);
		return(-1);
	    }
	    if(kana) {
		if((xjc->r_f = XLoadQueryFont(dpy, xjc->r_font)) == NULL)
			if((xjc->h_f = XLoadQueryFont(dpy, xjc->r_font)) == NULL) {
				fprintf(stderr, "Could not open font %s\n", xjc->r_font);
				send_err(xjc->w, XJP_F_OPEN_BAD_FN);
				free(xjc);
				return(-1);
			}
		xgcv.foreground = xjc->fg;
		xgcv.background = xjc->bg;
		xjc->gcr = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->gcr, xjc->r_f->fid);
		xgcv.foreground = xjc->bg;
		xgcv.background = xjc->fg;
		xjc->reversegcr = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->reversegcr, xjc->r_f->fid);
	    } else {
		send_err(xjc->w, XJP_F_OPEN_BAD_FN);
		free(xjc);
		return(-1);
	    }
	    /*	外字ＧＣ	*/
	    if(gaiji) {
		if((xjc->g_f = XLoadQueryFont(dpy, xjc->g_font)) == NULL)
			if((xjc->g_f = XLoadQueryFont(dpy, xjc->g_font)) == NULL) {
				fprintf(stderr, "Could not open font %s\n", xjc->g_font);
				return(-1);
			}
		xgcv.foreground = xjc->fg;
		xgcv.background = xjc->bg;
		xjc->gcg = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->gcg, xjc->g_f->fid);
		xgcv.foreground = xjc->bg;
		xgcv.background = xjc->fg;
		xjc->reversegcg = XCreateGC(dpy, DefaultRootWindow(dpy),
				(GCForeground|GCBackground), &xgcv);
		XSetFont(dpy, xjc->reversegcg, xjc->g_f->fid);
	    }
	}
	if (xjc->dispmode == 0) {
	    if (xjc->p_width <= 0) {
		send_err(xjc->w, XJP_F_OPEN_BAD_SZ);
		free(xjc);
		return(-1);
	    }
	    xjc->max_columns = xjc->p_width/FontWidth(xjc) - (MHL0 + 2);
	    xjc->columns = xjc->p_width/FontWidth(xjc);
	    if ((xjc->mask & XJP_BP) == 0)  xjc->bp = 1;
	    if ((xjc->mask & XJP_WIDTH) == 0)  xjc->width = xjc->p_width;
	    if ((xjc->mask & XJP_HEIGHT) == 0)  xjc->height = FontHeight(xjc);
	    if ((xjc->mask & XJP_X) == 0)  xjc->x = 0;
	    if ((xjc->mask & XJP_Y) == 0)  
			xjc->y = xjc->p_height -(xjc->height + xjc->bp);

	} else if (xjc->dispmode == 1) {
	    if (xjc->p_width <= 0) {
		send_err(xjc->w, XJP_F_OPEN_BAD_SZ);
		free(xjc);
		return(-1);
	    }
	    xjc->max_columns = xjc->p_width/FontWidth(xjc);
	    xjc->columns = xjc->p_width/FontWidth(xjc);
	    if ((xjc->mask & XJP_BP) == 0)  xjc->bp = 1;
	    if ((xjc->mask & XJP_HEIGHT) == 0)  xjc->height = FontHeight(xjc);
	}
	save_c_c = c_c;
	save_cur_p = cur_p;
	save_cur_x = cur_x;
	cur_x = xjc;
	if (xjc->dispmode != 2)
	    cur_p = xjc;
	else
	    cur_p = jfew->xjclient;
	if (new_client(win, xjc, 1) < 0) {
		fprintf("clients table over flow\n");
		free(xjc);
		c_c = save_c_c;
		cur_p = save_cur_p;
		cur_x = save_cur_x;
		return(-1);
	}
	cur_rk = c_c->rk;
	if (xjc->dispmode != 2) {
	    if (henkan_off_flag == 0) {
		xw_display_title_off();
		disp_mode();
	    } else {
		xw_display_title();
	    }
	}
	
	xjc->fd = -1;

	XSelectInput(dpy, xjc->w, StructureNotifyMask);
	if (xjc->dispmode != 2 && !jl_isconnect(bun_data_)) {
	    if (jserver_name_in_uumrc && *jserver_name_in_uumrc){
		print_msg_getc(" ｊｓｅｒｖｅｒ(at %s)と接続できませんでした。",
				jserver_name_in_uumrc);
	    } else {
		print_msg_getc(" ｊｓｅｒｖｅｒと接続できませんでした。");
	    }
	}
	c_c = save_c_c;
	cur_p = save_cur_p;
	cur_x = save_cur_x;
	cur_rk = c_c->rk;

	return(0);
	

}
/**********************************************************************
	クライアント用ウィンドウ作成 ルーチン
			from  cl_touroku()
**********************************************************************/
create_jw(xjc, sys, mhl0)
register XJClient	*xjc;
int	sys;
int	mhl0;
{
    int	width;
    int	height;
    int	border_width;
    int	ans;
    int	w_width;
    XWindowAttributes	attr;
    XSetWindowAttributes	ch_attr;
    XEvent			event;
    Window	root;
    Window	parent;
    Window	*child;
    unsigned int	nchild;
    register int	cnt;

    if (xjc->dispmode == 2){ /* デフォルト */
        xjc->wp1 = xjc->wp2 = xjc->wp3 = xjc->w1 = xjc->w2 = xjc->w3 = 0L;
	xjc->wp = xjc->wt = 0L;
        send_open(xjc, jfew->xjclient->wp);
        return(0);
    } else if (xjc->dispmode == 0){ /* ウィンドウ下部 */
	if((xjc->buf0= (int *)malloc(ret = (sizeof(int) * (MHL0 * 2 + 1)))) == NULL){
		fprintf(stderr, "error malloc\n");
		send_err(xjc->w, XJP_F_OPEN_NOT_WIN);
		return(-1);
	}
	if((xjc->buf= (int *)malloc(ret = (sizeof(int) * (maxchg * 2 + 1)))) == NULL){
		fprintf(stderr, "error malloc\n");
		send_err(xjc->w, XJP_F_OPEN_NOT_WIN);
		return(-1);
	}
	for (cnt = (maxchg * 2); cnt >= 0; cnt--) {
		*(xjc->buf + cnt) = 0;
	}
	clear_xjc(xjc);

	height = xjc->h_f->ascent + xjc->h_f->descent;
#ifdef DEBUG
	border_width = 1;
#else
	border_width = 0;
#endif
	xjc->del_x = 0;
	xjc->firstx = 0;
	xjc->firsty = (xjc->height - FontHeight(xjc))/2;
	xjc->maxcolumns = maxchg * 2;

		
	xjc->wp = XCreateSimpleWindow(dpy, xjc->w,
			xjc->x - xjc->bp, xjc->y, xjc->width, xjc->height,
			xjc->bp, xjc->fg, xjc->bg);

	xjc->wp1 = XCreateSimpleWindow(dpy, xjc->wp, 
			FontWidth(xjc) * (mhl0 + 1), xjc->firsty,
			FontWidth(xjc) * xjc->max_columns, height,
			border_width, xjc->fg, xjc->bg);
	xjc->wp2 = xjc->wp3 = 0;

	xjc->wnf = XCreateSimpleWindow(dpy, xjc->wp,
			FontWidth(xjc) * mhl0, xjc->firsty,
			FontWidth(xjc), height,
			border_width, xjc->fg, xjc->bg);

	xjc->wnb = XCreateSimpleWindow(dpy, xjc->wp,
			FontWidth(xjc) * (xjc->max_columns + mhl0 + 1),
			xjc->firsty,
			FontWidth(xjc), height,
			border_width, xjc->fg, xjc->bg);

	xjc->wnn = XCreateSimpleWindow(dpy, xjc->wp,
			FontWidth(xjc) * (xjc->max_columns + mhl0),
			xjc->firsty,
			FontWidth(xjc), height,
			border_width, xjc->fg, xjc->bg);

	xjc->w1 = XCreateSimpleWindow(dpy, xjc->wp1,
			0, 0,
			FontWidth(xjc) * (xjc->maxcolumns + 1), height,
			border_width, xjc->fg, xjc->bg);

	xjc->ws = XCreateSimpleWindow(dpy, xjc->wp,
			0, xjc->firsty, FontWidth(xjc) *  MHL0, height, 
			border_width, xjc->fg, xjc->bg);
	XDefineCursor(dpy, xjc->ws, cursor2);

	xjc->wt = XCreateSimpleWindow(dpy, xjc->wp,
			0, 0, xjc->width, xjc->height,
			border_width, xjc->fg, xjc->bg);

	if (!xjc->wp || !xjc->wp1 || !xjc->w1 || !xjc->wt || !xjc->wnf ||
	    !xjc->wnb || !xjc->wnn || !xjc->ws) {
		send_err(xjc->w, XJP_F_OPEN_NOT_WIN);
		return(-1);
	}

	xjc->w2 = xjc->w3 = 0;
	xjc->vst = 0;
	xjc->note_fore = 0;
	xjc->note_back = 0;

	ch_attr.save_under = True;
	ch_attr.backing_store = Always;
	XChangeWindowAttributes(dpy, xjc->wp, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->w1, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wnf, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wnb, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wt, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->ws, CWBackingStore, &ch_attr);

	XSelectInput(dpy, xjc->wp, KeyPressMask);

	XSelectInput(dpy, xjc->w1, ExposureMask);
	XSelectInput(dpy, xjc->wnf, ExposureMask);
	XSelectInput(dpy, xjc->wnb, ExposureMask);
	XSelectInput(dpy, xjc->wt, ExposureMask);
	XSelectInput(dpy, xjc->ws, 
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);

	redraw_windowt(xjc);

	if (!sys)
		send_open(xjc, xjc->w1);
	XMapWindow(dpy, xjc->ws);
	return(0);

    }else if (xjc->dispmode == 1){	/* カーソル位置(可変方式) */
#ifdef DEBUG
	border_width = 1;
#else
	border_width = 0;
#endif
	if((xjc->buf0= (int *)malloc(ret = (sizeof(int) * (MHL0 * 2 + 1)))) == NULL){
		fprintf(stderr, "error malloc\n");
		send_err(xjc->w, XJP_F_OPEN_NOT_WIN);
		return(-1);
	}
	if((xjc->buf= (int *)malloc(ret = (sizeof(int) * (maxchg * 2 + 1)))) == NULL){
		fprintf(stderr, "error malloc\n");
		send_err(xjc->w, XJP_F_OPEN_NOT_WIN);
		return(-1);
	}
	for (cnt = maxchg * 2; cnt >= 0; cnt--) {
		*(xjc->buf + cnt) = 0;
	}
	clear_xjc(xjc);

	xjc->maxcolumns = maxchg * 2;
	width = xjc->h_f->max_bounds.width * (xjc->maxcolumns + 1);
	height = xjc->h_f->ascent + xjc->h_f->descent;
	xjc->del_x = 0;
	xjc->firstx = 0;
	xjc->firsty = (xjc->height - FontHeight(xjc))/2;
	xjc->vst = 0;
	xjc->note_fore = 0;
	xjc->note_back = 0;

	xjc->wp1 = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, xjc->p_width, height,border_width,
			xjc->fg, xjc->bg);
	xjc->wp2 = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, xjc->p_width, height,border_width,
			xjc->fg, xjc->bg);
	xjc->wp3 = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, xjc->p_width, height,border_width,
			xjc->fg, xjc->bg);
	xjc->w1 = XCreateSimpleWindow(dpy, xjc->wp1,
			0, 0, width, height,border_width,
			xjc->fg, xjc->bg);
	xjc->w2 = XCreateSimpleWindow(dpy, xjc->wp2,
			0, 0, width, height,border_width,
			xjc->fg, xjc->bg);
	xjc->w3 = XCreateSimpleWindow(dpy, xjc->wp3,
			0, 0, width, height,border_width,
			xjc->fg, xjc->bg);
	xjc->wnf = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, FontWidth(xjc), height,
			border_width, xjc->fg, xjc->bg);
	xjc->wnb = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, FontWidth(xjc), height,
			border_width, xjc->fg, xjc->bg);
	xjc->wnn = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, FontWidth(xjc), height,
			border_width, xjc->fg, xjc->bg);
	xjc->ws = XCreateSimpleWindow(dpy, xjc->w,
			0, 0, FontWidth(xjc) *  MHL0, height, xjc->bp,
			xjc->fg, xjc->bg);

	if (!xjc->wp1 || !xjc->wp2 || !xjc->wp3 || !xjc->w1 || !xjc->w2 || 
	    !xjc->w3 || !xjc->wnf || !xjc->wnb || !xjc->wnn || !xjc->ws) {
		send_err(xjc->w, XJP_F_OPEN_NOT_WIN);
		return(-1);
	}

	XDefineCursor(dpy, xjc->ws, cursor2);

	ch_attr.bit_gravity = NorthWestGravity;
	XChangeWindowAttributes(dpy, xjc->wp1, CWBitGravity, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wp2, CWBitGravity, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wp3, CWBitGravity, &ch_attr);
	ch_attr.backing_store = Always;
	ch_attr.save_under = True;
	XChangeWindowAttributes(dpy, xjc->w1,
			CWBitGravity, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->w2,
			CWBitGravity, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->w3,
			CWBitGravity, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wnf,
			CWBitGravity | CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wnb,
			CWBitGravity | CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->ws,
			CWBitGravity | CWBackingStore, &ch_attr);
	XSelectInput(dpy, xjc->w1, ExposureMask);
	XSelectInput(dpy, xjc->w2, ExposureMask);
	XSelectInput(dpy, xjc->w3, ExposureMask);
	XSelectInput(dpy, xjc->wp1, KeyPressMask);
	XSelectInput(dpy, xjc->wp2, KeyPressMask);
	XSelectInput(dpy, xjc->wp3, KeyPressMask);
	XSelectInput(dpy, xjc->ws,
		KeyPressMask | ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);

	xjc->c0 = xjc->c1 = xjc->c2 = xjc->maxcolumns;
	XMapWindow(dpy, xjc->w1);
	XMapWindow(dpy, xjc->w2);
	XMapWindow(dpy, xjc->w3);
	XFlush(dpy);
	send_open(xjc, xjc->w1);
	return(0);
    }else{
	fprintf("dispmode が間違っています。\n");
	send_err(xjc->w, XJP_F_OPEN_NOT_SUPPORT);
	return(-1);
    }
}

send_open(xjc, w)
register XJClient	*xjc;
Window		w;
{
	XEvent	event;

	event.type = ClientMessage;
	event.xclient.format = 32;
	event.xclient.window = w;
	event.xclient.data.l[0] = XJP_OPEN;
	event.xclient.data.l[1] = w;
	event.xclient.data.l[2] = FontWidth(xjc) * MHL0;
	event.xclient.data.l[3] = FontHeight(xjc);
	XSendEvent(dpy, xjc->w, False, NoEventMask, &event);
	XFlush(dpy);
	return(0);
}

/********************************************************************
	クライアント属性変更ルーチン
			from xw_read()
********************************************************************/
cl_change(ch_w)
Window	ch_w;
{
	register int	cnt;
	Atom	ch_atom;

	for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if(client_mem[cnt] != NULL && ch_w == client_mem[cnt]->w1){
			ch_atom = XInternAtom (dpy, "XJPCLIENT", True);
			if(change_jw(client_mem[cnt], ch_atom)){
				return(-1);
			}
			return(0);
		}
	}
	return(-1);
}

cl_move(ch_w, x, y)
Window ch_w;
int x, y;
{
    register int	cnt;
    struct wnmo_client *save_c_c;
    XJClient *save_cur_p, *save_cur_x;

    for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
	if(client_mem[cnt] != NULL && ch_w == client_mem[cnt]->w1){
    	    if (client_mem[cnt]->dispmode == 1) {
		client_mem[cnt]->x = x;
		client_mem[cnt]->y = y;
		save_c_c = c_c;
		save_cur_p = cur_p;
		save_cur_x = cur_x;
		cur_p = client_mem[cnt];
		if (!henkan_off_flag) {
		    if(!ifempty() && cur_x == client_mem[cnt]){
			invisual_window1();
			client_mem[cnt]->visible = 0;
			visual_window1();
			check_scroll();
		    }
		    xw_disp_mode1();
		}
		c_c = save_c_c;
		cur_p = save_cur_p;
		cur_x = save_cur_x;
		return(0);
	    }
	    return(-1);
	}
    }
    return(-1);
}

/********************************************************************
	クライアント用ウィンドウ属性変更ルーチン
			from cl_change()
********************************************************************/
change_jw(xjc, ch_atom)
register XJClient	*xjc;
Atom		ch_atom;
{
	Atom		actual_type;
	int		actual_format;
	unsigned long	nitems;
	unsigned long	byte_after;
	unsigned long	leftover;
	unsigned char	*data;
	register XJClient	savexjc;
	int		ch_flag = 0;
	unsigned long	mask;
	int	ch_col_flag = 0;
	int	ch_size_flag = 0;
	int	ch_status_flag = 0;
	struct wnmo_client *save_c_c;
	XJClient *save_cur_p, *save_cur_x;

	XGetWindowProperty(dpy, xjc->w, ch_atom, 0L, 1000000L, False, XA_STRING, 
			&actual_type, &actual_format, &nitems, &leftover, &data); 
	bcopy(data, &savexjc, 184);

	mask = savexjc.mask;
	if ((mask & 0x40) || (mask & 0x80) || (mask & 0x100)) {
		if(mask & 0x40 && xjc->fg != savexjc.fg) {
			 xjc->fg = savexjc.fg;
			ch_col_flag = 1;
		}
		if(mask & 0x80 && xjc->bg != savexjc.bg) {
			xjc->bg = savexjc.bg;
			ch_col_flag = 1;
		}
		if(mask & 0x100 && xjc->bp != savexjc.bp) {
			xjc->bp = savexjc.bp;
			ch_col_flag = 1;
		}
	}
	if ((mask & 0x1) || (mask & 0x2) || (mask & 0x4) || (mask & 0x8) || (mask & 0x10) || (mask & 0x20)) {
		if(mask & 0x1 && xjc->p_width != savexjc.p_width) {
			xjc->p_width = savexjc.p_width;
			ch_size_flag = 1;
		}
		if(mask & 0x2 && xjc->p_height != savexjc.p_height) {
			xjc->p_height = savexjc.p_height;
			ch_size_flag = 1;
		}
		if(mask & 0x4 && xjc->x != savexjc.x) {
			xjc->x = savexjc.x;
			ch_size_flag = 1;
		}
		if(mask & 0x8 && xjc->y != savexjc.y) {
			xjc->y = savexjc.y;
			ch_size_flag = 1;
		}
		if(mask & 0x10 && xjc->width != savexjc.width) {
			xjc->width = savexjc.width;
			ch_size_flag = 1;
		}
		if(mask & 0x20 && xjc->height != savexjc.height) {
			xjc->height = savexjc.height;
			ch_size_flag = 1;
		}
	}
	if (ch_col_flag)
		change_color(xjc);
	if (ch_size_flag)
		change_size(xjc);
	if(mask & 0x400 && xjc->dispmode == 1) {
		xjc->status_x = savexjc.status_x;
		xjc->status_y = savexjc.status_y;
		ch_status_flag = 1;
		change_status(xjc);
	}
	if (ch_col_flag || ch_size_flag || ch_status_flag) {
	    save_c_c = c_c;
	    save_cur_p = cur_p;
	    save_cur_x = cur_x;
	    cur_p = cur_x = xjc;
	    c_c = cur_p->w_c;
	    cur_rk = c_c->rk;
	    if (xjc->dispmode == 1) {
		if (!henkan_off_flag) {
		    if (!ifempty()) {
			invisual_window1();
			xjc->visible = 0;
			visual_window1();
			check_scroll();
		    }
		    disp_mode();
		    xw_disp_mode1();
		}
	    } else if (xjc->dispmode == 0) {
		    if (!ifempty()) {
			invisual_window();
			xjc->visible = 0;
			visual_window();
		    }
		    disp_mode();
	    }
	    c_c = save_c_c;
	    cur_p = save_cur_p;
	    cur_x = save_cur_x;
	    cur_rk = c_c->rk;
	}
}

/********************************************************************
	クライアント用ウィンドウカラー変更ルーチン
			from change_jw()
********************************************************************/
change_color(xjc)
register XJClient	*xjc;
{
	XGCValues	values;
	struct wnmo_client *save_c_c;
	XJClient *save_cur_p;

	if (xjc->dispmode == 1 || xjc->dispmode == 0) {
		values.foreground = xjc->fg;
		values.background = xjc->bg;
		if (xjc->gch)
			XChangeGC (dpy, xjc->gch,
					GCForeground | GCBackground, &values);
		if (xjc->gck)
			XChangeGC (dpy, xjc->gck,
					GCForeground | GCBackground, &values);
		if (xjc->gcg)
			XChangeGC (dpy, xjc->gcg,
					GCForeground | GCBackground, &values);
		values.foreground = xjc->bg;
		values.background = xjc->fg;
		if (xjc->reversegch)
			XChangeGC (dpy, xjc->reversegch,
					GCForeground | GCBackground, &values);
		if (xjc->reversegck)
			XChangeGC (dpy, xjc->reversegck,
					GCForeground | GCBackground, &values);
		if (xjc->reversegcg)
			XChangeGC (dpy, xjc->reversegcg,
					GCForeground | GCBackground, &values);
		XFlush(dpy);
		if (xjc->dispmode == 0) {
			save_c_c = c_c;
			save_cur_p = cur_p;
			cur_p = xjc;
			XUnmapWindow(dpy, xjc->wp);
			XSetWindowBackground (dpy, xjc->wp, xjc->bg);
			XSetWindowBorder (dpy, xjc->wp, xjc->fg);
			XMapWindow(dpy, xjc->wp);
			XSetWindowBackground (dpy, xjc->wp1, xjc->bg);
			XSetWindowBackground (dpy, xjc->w1, xjc->bg);
			XSetWindowBackground (dpy, xjc->wnf, xjc->bg);
			XSetWindowBackground (dpy, xjc->wnb, xjc->bg);
			XSetWindowBackground (dpy, xjc->wnn, xjc->bg);
			XSetWindowBackground (dpy, xjc->ws, xjc->bg);
			XSetWindowBackground (dpy, xjc->wt, xjc->bg);
			XClearWindow(dpy, xjc->w1);
			XClearWindow(dpy, xjc->wt);
			redraw_windowt(xjc);
			disp_mode();
			XFlush(dpy);
			c_c = save_c_c;
			cur_p = save_cur_p;
		} else if (xjc->dispmode == 1) {
			save_c_c = c_c;
			save_cur_p = cur_p;
			cur_p = xjc;
			XFlush(dpy);
			XSetWindowBackground (dpy, xjc->w1, xjc->bg);
			XSetWindowBackground (dpy, xjc->w2, xjc->bg);
			XSetWindowBackground (dpy, xjc->w3, xjc->bg);
			XSetWindowBorder (dpy, xjc->wp1, xjc->fg);
			XSetWindowBorder (dpy, xjc->wp2, xjc->fg);
			XSetWindowBorder (dpy, xjc->wp3, xjc->fg);
			XSetWindowBackground (dpy, xjc->wnf, xjc->bg);
			XSetWindowBackground (dpy, xjc->wnb, xjc->bg);
			XSetWindowBackground (dpy, xjc->wnn, xjc->bg);
			XSetWindowBackground (dpy, xjc->ws, xjc->bg);
			disp_mode();
			XFlush(dpy);
			c_c = save_c_c;
			cur_p = save_cur_p;
		}
		
		XFlush(dpy);
	}
}
	
/********************************************************************
	クライアント用ウィンドウサイズ変更ルーチン
			from change_jw()
********************************************************************/
change_size(xjc)
register XJClient	*xjc;
{
	int	height, border_width, w_width;
	XEvent	event;

	if (xjc->dispmode == 0){
		XUnmapWindow(dpy, xjc->wp);
		height = xjc->h_f->ascent + xjc->h_f->descent;
		border_width = 0;
		xjc->max_columns = xjc->p_width/FontWidth(xjc) - (MHL0 + 2);
		XMoveResizeWindow(dpy, xjc->wp,
			xjc->x - xjc->bp, xjc->y,
			xjc->width, xjc->height);
		XMoveResizeWindow(dpy, xjc->wt,
			0, 0,
			xjc->width, xjc->height);
		XMoveResizeWindow(dpy, xjc->wp1,
			FontWidth(xjc) * (MHL0 + 1), xjc->firsty,
			FontWidth(xjc) * xjc->max_columns, height);
		XMoveResizeWindow(dpy, xjc->w1,
			0, 0,
			FontWidth(xjc) * (xjc->maxcolumns + 1),height);
		XMoveResizeWindow(dpy, xjc->wnf,
			FontWidth(xjc) * MHL0, xjc->firsty,
			FontWidth(xjc), height);
		XMoveResizeWindow(dpy, xjc->wnb,
			FontWidth(xjc) * (xjc->max_columns+MHL0+1), xjc->firsty,
			FontWidth(xjc), height);
		XMoveResizeWindow(dpy, xjc->wnn,
			FontWidth(xjc) * (xjc->max_columns + MHL0), xjc->firsty,
			FontWidth(xjc), height);

		XMapWindow(dpy, xjc->wp);
		XFlush(dpy);
	}
}

change_status(xjc)
register XJClient *xjc;
{
	XMoveWindow(dpy, xjc->ws, xjc->status_x, xjc->status_y);
	XRaiseWindow(dpy, xjc->ws);
	XFlush(dpy);
	xjc->status_lock = 1;
}

/********************************************************************
	クライアント削除ルーチン
			from xw_read()
********************************************************************/
cl_sakujo(set_w)
Window	set_w;
{
    register int	cnt;
    register int	cntt;
    register int	cnttt;

    for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
	if(client_mem[cnt] != NULL && set_w == client_mem[cnt]->w){
    	    if (client_mem[cnt]->re_count == 0) {
		if (cur_x == client_mem[cnt]) {
    		    invisual_window();
    		    cur_input = NULL;
    		    cur_x = NULL;
    		    cur_p = NULL;
		}
    		if(destroy_jw(client_mem[cnt])){
		    return(-1);
    		}
    		XSelectInput(dpy, client_mem[cnt]->w, NoEventMask);
    		if (client_mem[cnt]->dispmode != 2)
		    del_client(client_mem[cnt]->w_c);
    		for(cntt = (input_num - 1); cntt >= 0; cntt--) {
			if (input_mem[cntt]->pclient == client_mem[cnt]) {
			XSelectInput(dpy, input_mem[cntt]->w, NoEventMask);
			free(input_mem[cntt]);
			for(cnttt = cntt; cnttt < (input_num - 1); cnttt++) {
			    input_mem[cnttt] = input_mem[cnttt + 1];
			}
			input_num--;
		    }
		}
		free(client_mem[cnt]);
		client_mem[cnt] = NULL;
	    } else {
    		client_mem[cnt]->re_count--;
	    }
	    return(0);
	}
    }
	return(-1);
}

cl_destroy(set_w)
Window	set_w;
{
	register int	cnt;
	register int	cntt;
	register int	cnttt;
	for (cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if(client_mem[cnt] != NULL && set_w == client_mem[cnt]->w){
			if (cur_x == client_mem[cnt]) {
				invisual_window();
				cur_input = NULL;
				cur_x = NULL;
				cur_p = NULL;
			}
			if(destroy_jw(client_mem[cnt])){
				return(-1);
			}
    			if (client_mem[cnt]->dispmode != 2)
				del_client(client_mem[cnt]->w_c);
			for(cntt = 0; cntt < input_num; cntt++) {
				if (input_mem[cntt]->pclient == client_mem[cnt]) {
					free(input_mem[cntt]);
					for(cnttt = cntt; cnttt < (input_num - 1); cnttt++) {
						input_mem[cnttt] = input_mem[cnttt + 1];
					}
					input_num--;
				}
			}
			free(client_mem[cnt]);
			client_mem[cnt] = NULL;
			return(0);
		}
	}
	return(-1);
}

/********************************************************************
	クライアント用ウィンドウ削除ルーチン
			from cl_sakujo()
********************************************************************/
destroy_jw(xjc)
register XJClient	*xjc;
{
	int	mode;
	XSetWindowAttributes	ch_attr;
	XWindowAttributes	attr;

	if (xjc->dispmode != 2) {
		if (xjc->dispmode == 1) {
			XDestroyWindow(dpy, xjc->wp2);
			XDestroyWindow(dpy, xjc->wp3);
			XDestroyWindow(dpy, xjc->w2);
			XDestroyWindow(dpy, xjc->w3);
		} else if (xjc->dispmode == 0) {
			XDestroyWindow(dpy, xjc->wp);
		}
		XDestroyWindow(dpy, xjc->w1);
		XDestroyWindow(dpy, xjc->wnf);
		XDestroyWindow(dpy, xjc->wnb);
		XDestroyWindow(dpy, xjc->wnn);
		XDestroyWindow(dpy, xjc->wp1);
		XDestroyWindow(dpy, xjc->ws);
	} else if (xjc->dispmode == 2) {
#ifdef	DIRECT
		if (xjc->fd != -1)
			close_socket(xjc->fd);
#endif	/* DIRECT */
	} else {
		fprintf(stderr,"dispmode が間違っています。\n");
		return(-1);
	}
	XFlush(dpy);
	free(xjc);
	return(0);
}

/*********************************************************************
	変換開始 カレントクライアントセット ルーチン
			from xw_read()
*********************************************************************/
cur_cl_set(set_window)
Window	set_window;
{
	register int	cnt;

	for( cnt = (input_num - 1) ; cnt >= 0; cnt--){
		if(set_window == input_mem[cnt]->w
		|| set_window == input_mem[cnt]->pclient->wp1
		|| set_window == input_mem[cnt]->pclient->wp2
		|| set_window == input_mem[cnt]->pclient->wp3
		|| set_window == input_mem[cnt]->pclient->wnf
		|| set_window == input_mem[cnt]->pclient->wnb
		|| set_window == input_mem[cnt]->pclient->wp) {
			cur_x = input_mem[cnt]->pclient;
			if (cur_x->dispmode == 2) {
				cur_p = jfew->xjclient;
				c_c = jfew->xjclient->w_c;
			} else {
				cur_p = cur_x;
				c_c = cur_x->w_c;
			}
			cur_rk = c_c->rk;
			cur_input = input_mem[cnt];
			return(0);
		}
	}
	if ((set_window == jfew->ichi->w || set_window == jfew->inspect->w)
	    && jfew->j_c != NULL) {
		cur_p = jfew->j_c;
		c_c = cur_p->w_c;
		cur_rk = c_c->rk;
		return(0);
	}
	return(-1);
}
cur_cl_change(set_window)
Window	set_window;
{
	register int	cnt;

	for( cnt = (input_num - 1) ; cnt >= 0; cnt--){
		if(set_window == input_mem[cnt]->w
		|| set_window == input_mem[cnt]->pclient->wp1
		|| set_window == input_mem[cnt]->pclient->wp2
		|| set_window == input_mem[cnt]->pclient->wp3
		|| set_window == input_mem[cnt]->pclient->wnf
		|| set_window == input_mem[cnt]->pclient->wnb
		|| set_window == input_mem[cnt]->pclient->ws
		|| set_window == input_mem[cnt]->pclient->wp) {
			cur_x = input_mem[cnt]->pclient;
			if (cur_x->dispmode == 2) {
				cur_p = jfew->xjclient;
				c_c = jfew->xjclient->w_c;
			} else {
				cur_p = cur_x;
				c_c = cur_x->w_c;
			}
			cur_rk = c_c->rk;
			cur_input = input_mem[cnt];
			return(0);
		}
	}
	if ((set_window == jfew->ichi->w || set_window == jfew->inspect->w)
	    && jfew->j_c != NULL) {
		cur_p = jfew->j_c;
		c_c = cur_p->w_c;
		cur_rk = c_c->rk;
		return(0);
	}
	return(-1);
}

cur_cl_set2(set_window)
Window	set_window;
{
	register int	cnt;

	for(cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if(client_mem[cnt] != NULL && set_window == client_mem[cnt]->w){
			return(0);
		}
	}
	return(-1);
}

map_window3(set_window)
Window set_window;
{
	register int	cnt;

	for(cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if(client_mem[cnt] != NULL && set_window == client_mem[cnt]->w){
			map_window2(client_mem[cnt]);
			return(0);
		}
	}
	return(-1);
}

map_window2(xjc)
register XJClient	*xjc;
{
    XJClient *save_cur_p, *save_cur_x;

    if (xjc->dispmode == 0) {
	XMapWindow(dpy, xjc->wp);
	XFlush(dpy);
    } else if (xjc->dispmode == 1) {
	if (xjc->currentcol > 0 && xjc->visible == 0) {
	save_cur_p = cur_p;
	save_cur_x = cur_x;
	cur_p = cur_x = xjc;
	xw_display_title_off();
	visual_window1();
	cur_p = save_cur_p;
	cur_x = save_cur_x;
	}
    }
}

unmap_window3(set_window)
Window set_window;
{
	register int	cnt;

	for(cnt = 1; cnt < MAX_CLIENTS; cnt++) {
		if(client_mem[cnt] != NULL && set_window == client_mem[cnt]->w){
			unmap_window2(client_mem[cnt]);
			return(0);
		}
	}
	return(-1);
}

unmap_window2(xjc)
XJClient *xjc;
{
    XJClient *save_cur_p, *save_cur_x;

    if (xjc->dispmode == 0) {
	XUnmapWindow(dpy, xjc->wp);
	XFlush(dpy);
    } else if (xjc->dispmode == 1) {
	save_cur_p = cur_p;
	save_cur_x = cur_x;
	cur_p = cur_x = xjc;
	xw_display_title();
	invisual_window1();
	cur_p = save_cur_p;
	cur_x = save_cur_x;
    }
}

check_kan(buf,x)
register int	*buf;
register int	x;
{
	if ((*(buf + x) & KAN1) != 0)
		return(1);
	else if ((*(buf + x) & KAN2) != 0)
		return(2);
	else
		return(0);
}


JWwindow_move(xjc)
register XJClient	*xjc;
{

	if ((check_kan(xjc->buf, xjc->vst)) == 2)
		xjc->vst++;
	XMoveWindow(dpy, xjc->w1, -(xjc->vst * FontWidth(xjc)), 0);
	if (cur_x->dispmode == 1) {
		XMoveWindow(dpy, xjc->w2,
			-((xjc->vst - xjc->linefeed1) * FontWidth(xjc)), 0);
		XMoveWindow(dpy, xjc->w3,
			-((xjc->vst - xjc->linefeed2) * FontWidth(xjc)), 0);
	}
	XFlush(dpy);
}

check_move(xjc,kan)
register XJClient	*xjc;
int	kan;
{
	int	currentcol_tmp;

	if (cur_x->dispmode == 1) {
		check_move1(xjc, kan);
	} else {
		if ((xjc->currentcol - xjc->vst + kan) >= xjc->max_columns) {
			xjc->vst = xjc->currentcol - xjc->max_columns + kan + 1;
			JWwindow_move(xjc);
		} else if (xjc->currentcol < xjc->vst) {
			xjc->vst = xjc->currentcol;
			JWwindow_move(xjc);
		}
		if (xjc->vst) {
			JWM_note_fore();
			xjc->note_fore = 1;
		} else {
			invisual_note_fore();
			xjc->note_fore = 0;
		}
		if (xjc->max_pos > (xjc->vst + xjc->max_columns)) {
			JWM_note_back();
			xjc->note_back = 1;
			if ((check_kan(xjc->buf,
					xjc->vst + xjc->max_columns - 1)) == 1)
				XMapWindow(dpy, xjc->wnn);
			else
				XUnmapWindow(dpy, xjc->wnn);
		} else {
			invisual_note_back();
			xjc->note_back = 0;
			XUnmapWindow(dpy, xjc->wnn);
		}
	}
}
check_move1(xjc,kan)
register XJClient	*xjc;
int	kan;
{
	int	currentcol_tmp;
	int	max_pos_tmp;

	if (xjc->visible_line == 3) {
		currentcol_tmp = cur_x->currentcol + xjc->linefeed2;
	} else if (xjc->visible_line == 2) {
		currentcol_tmp = xjc->currentcol + xjc->linefeed1;
	} else {
		currentcol_tmp = xjc->currentcol;
	}

	if ((currentcol_tmp - xjc->vst + kan) >= xjc->max_columns) {
		do {
			xjc->vst = currentcol_tmp - xjc->max_columns + kan + 1;
			if ((check_kan(xjc->buf, xjc->vst)) == 2)
				xjc->vst++;
			currentcol_tmp = xjc->currentcol;
			if (xjc->visible_line > 1) {
				if ((check_kan(xjc->buf,
						xjc->c0 + xjc->vst -1)) == 1) {
					xjc->linefeed1 = 1;
				} else {
					xjc->linefeed1 = 0;
				}
				currentcol_tmp = xjc->currentcol + xjc->linefeed1;
			}
			if (xjc->visible_line > 2) {
				if ((check_kan(xjc->buf,
					xjc->c0 + xjc->c1 + xjc->vst -xjc->linefeed1 - 1)) == 1) {
					xjc->linefeed2 = xjc->linefeed1 + 1;
					} else {
					xjc->linefeed2 = xjc->linefeed1;
					}
				currentcol_tmp = xjc->currentcol + xjc->linefeed2;
			}
		}
		while ((currentcol_tmp - xjc->vst + kan) >= xjc->max_columns);
		JWwindow_move(xjc);
	} else if (xjc->currentcol < xjc->vst) {
		xjc->vst = xjc->currentcol;
		if (xjc->visible_line > 1
		    || (xjc->vst + xjc->c0 < xjc->max_pos)) {
			if ((check_kan(xjc->buf, xjc->c0 + xjc->vst -1)) == 1) {
				xjc->linefeed1 = 1;
			} else {
				xjc->linefeed1 = 0;
			}
			if (xjc->visible_line < 2
			    && (xjc->vst + xjc->c0 < xjc->max_pos)) {
				visible_line2();
				xjc->visible_line = 2;
			}
		}
		if (xjc->visible_line > 2
		    || (xjc->vst + xjc->c0 + xjc->c1 - xjc->linefeed1 < xjc->max_pos)) {
			if ((check_kan(xjc->buf, xjc->c0 + xjc->c1 + xjc->vst -xjc->linefeed1 - 1)) == 1) {
				xjc->linefeed2 = xjc->linefeed1 + 1;
			} else {
				xjc->linefeed2 = xjc->linefeed1;
			}
			if (xjc->visible_line < 3
		    	    && (xjc->vst + xjc->c0 + xjc->c1 - xjc->linefeed1 < xjc->max_pos)) {
				visible_line3();
				xjc->visible_line = 3;
			}
		}
		JWwindow_move(xjc);
	}

	if (xjc->vst) {
		if (!xjc->note_fore) {
			JWM_note_fore();
			xjc->note_fore = 1;
		}
	} else {
		if (!xjc->note_back) {
			invisual_note_fore();
			xjc->note_fore = 0;
		}
	}
	if (xjc->visible_line == 3)
		max_pos_tmp = xjc->vst + xjc->max_columns - xjc->linefeed2;
	else if (xjc->visible_line == 2)
		max_pos_tmp = xjc->vst + xjc->max_columns - xjc->linefeed1;
	else
		max_pos_tmp = xjc->vst + xjc->max_columns;

	if (xjc->max_pos > max_pos_tmp) {
		if (!xjc->note_back) {
			JWM_note_back();
			xjc->note_back = 1;
		}
		if ((check_kan(xjc->buf,max_pos_tmp - 1)) == 1)
			JWM_note_null();
		else
			XUnmapWindow(dpy, xjc->wnn);
	} else {
		if (xjc->note_back) {
			invisual_note_back();
			xjc->note_back = 0;
		}
		XUnmapWindow(dpy, xjc->wnn);
	}
}

JWM_note_null()
{
	register XJClient	*xjc;
	int	x, y;

	xjc = cur_p;

	if (xjc->visible_line > 2) {
		x = (xjc->c2 - 1) * FontWidth(xjc);
		y = xjc->y + FontHeight(xjc) * 2;
		if ((y + FontHeight(xjc) * 2) > xjc->p_height)
			if ((y + FontHeight(xjc)) > xjc->p_height)
				y = FontHeight(xjc);

			else
				y = 0;
	} else if (xjc->visible_line > 1) {
		x = (xjc->c1 - 1) * FontWidth(xjc);
		y = xjc->y + FontHeight(xjc);
		if ((y + FontHeight(xjc)) > xjc->p_height)
			y = 0;
	} else {
		x = xjc->x + (xjc->c0 - 1) * FontWidth(xjc);
		y = xjc->y;
	}
	XMoveWindow(dpy, xjc->wnn, x, y);
	XMapWindow(dpy, xjc->wnn);
}

check_scroll()
{
	XJClient	*xjc;
	int		kan;

	xjc = cur_p;
	kan = check_kan(xjc->buf, xjc->currentcol);
	if (xjc->currentcol < xjc->vst
	|| (xjc->visible_line == 1 && (xjc->currentcol - xjc->vst - kan) >= xjc->max_columns)
	|| (xjc->visible_line == 2 && (xjc->currentcol - xjc->vst - kan + xjc->linefeed1) >= xjc->max_columns)
	|| (xjc->visible_line == 3 && (xjc->currentcol - xjc->vst - kan + xjc->linefeed2) >= xjc->max_columns)) {
		if (kan == 1)
			check_move(xjc, 1);
		else
			check_move(xjc, 0);
	}
}


xw_jishoadd()
{
    set_uum_env();
    send_dic_event(DIC_ADD);
    return(0);
}

xw_kensaku()
{
    set_uum_env();
    send_dic_event(DIC_KENSAKU);
}

xw_select_one_dict9()
{
    set_uum_env();
    send_dic_event(DIC_ICHIRAN);
    return(0);
}

xw_paramchg()
{
    set_uum_env();
    send_dic_event(DIC_PARAM);
    return(0);
}

xw_dicinfoout()
{
    set_uum_env();
    send_dic_event(DIC_INFO);
    return(0);
}

xw_fuzoku_set()
{
    set_uum_env();
    send_dic_event(DIC_FZK);
    return(0);
}

xw_touroku(s)
int s;
{
    set_uum_touroku(s);
    send_dic_event(DIC_TOUROKU);
    return(0);
}


send_dic_event(id)
int	id;
{
	XEvent	event;

	event.type = ClientMessage;
	event.xclient.format = 32;
	event.xclient.window = jfew->xjclient->w;
	event.xclient.data.l[0] = id;
	XSendEvent(dpy, dicserver_id, False, NoEventMask, &event);
	XFlush(dpy);
	return(0);
}

set_dicserver()
{
    Atom id;

    id = XInternAtom(dpy, DICSERVER, False);
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
			PropModeReplace, dic_info, sizeof(DIC_SERV_INFO));
    XSetSelectionOwner(dpy, id, jfew->xjclient->wp,  0L);
    XFlush(dpy);
}

start_dicserver()
{
    char dic_serv[256];

    strcpy(dic_serv, DICSERVERNAME);
    execl(dic_serv,dic_serv, display_name, 0);
}

isstart_dicserver()
{
    int ret;
    if (ret = XInternAtom(dpy, "DICSERVER_OK", True)) {
	if (XGetSelectionOwner(dpy, ret)) {
	    return(1);
	}
    }
    return(0);
}

set_uum_env()
{
    Atom id;
    int tmp = -1;

    if((id = XInternAtom(dpy, DICENV, True)) == 0) {
	id = XInternAtom(dpy, DICENV, False);
    }
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
			PropModeReplace, &(bun_data_->env->env_id),
			sizeof(int));
    if (env_kankana)
	tmp = env_kankana->env_id;
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
			PropModeAppend, &(tmp),
			sizeof(int));
    XFlush(dpy);
}

set_uum_touroku(start)
int start;
{
    Atom id;
    int tmp = -1;

    if((id = XInternAtom(dpy, DICENV, True)) == 0) {
	id = XInternAtom(dpy, DICENV, False);
    }
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
    			PropModeReplace, &(bun_data_->env->env_id),
			sizeof(int));
    if (env_kankana)
	tmp = env_kankana->env_id;
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
			PropModeAppend, &(tmp),
			sizeof(int));
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
			PropModeAppend, &start,
			sizeof(int));
    XChangeProperty(dpy, DefaultRootWindow(dpy), id, XA_STRING, 8,
			PropModeAppend, c_b->buffer,
			Strlen(c_b->buffer)*2);
    
}

#undef	malloc

char *Salloc(size)
unsigned size;
{
	return((char *)calloc(1, size));
}
