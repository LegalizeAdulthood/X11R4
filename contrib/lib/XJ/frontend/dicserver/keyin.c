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
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define	XOR(a,b)	((a&(~b)) | ((~a)&b))

int	ret;
Display	*dpy = 0;			/* ディスプレイ構造体へのポインタ */
JFEWSTR	*jfew;		 	/* 自分自身の JFEWSTR 		*/
Jclient	*cur_jutil;
XJClient *cur_x;
unsigned long boderground, background, foreground;

char	display_name[MAXNAMLEN] = { 0 };/* ホストマシン名	*/
Cursor	cursor1, cursor2, cursor3;

Window window_jfew;
int save_env_id;

int	work = 0;
int	*cur_JW_buf;
int	button_x;
int	button_y;
int	select_return = -1;
extern	int	select_button;
Atom	motif_id = 0;		/* MOTIF */
Atom	motif_id1 = 0;		/* MOTIF */

int	ascii_font_id;
int	kanji_font_id;
int	kana_font_id;
/******************************************************************************
	ＸＷＮＭＯウィンドウ作成 ルーチン
			from main()
******************************************************************************/
create_jfew()
{
	register XJClient	*xjc;
	register Ichiran	*ichi;
	register YesOrNo	*yes_no;
	int		screen;
	XGCValues	xgcv;
	XSetWindowAttributes	attributes;
	extern	int	xerror(), xioerror();
	extern	char	*getenv();
	unsigned long pixel;
	register int	cnt;
	int ret;

	if((dpy = XOpenDisplay(display_name)) == NULL)
	{
		fprintf(stderr,"Could not open Display : %s", display_name);
		return(-1);
	}
	if (ret = XInternAtom(dpy, "DICSERVER_OK", True)) {
	    if (XGetSelectionOwner(dpy, ret)) {
		fprintf(stderr,"dicserver already running");
		return(-1);
	    }
	}
	if((xjc = (XJClient *)malloc(ret = sizeof(XJClient))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	clear_xjc(xjc);
	if((ichi = (Ichiran *)malloc(ret = sizeof(Ichiran))) == NULL){
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
	xjc->buf = NULL;
	jfew->xjclient = xjc;
	jfew->ichi = ichi;
	jfew->yes_no = yes_no;
	
	XSetErrorHandler(xerror);
	XSetIOErrorHandler(xioerror);

	screen = DefaultScreen(dpy);
        cursor1 = XCreateFontCursor(dpy, 52);
        cursor2 = XCreateFontCursor(dpy, 60);
        cursor3 = XCreateFontCursor(dpy, 24);

	get_property();

	if ((jfew->h_f = XQueryFont(dpy, ascii_font_id)) == NULL) {
		return(-1);
	}
	if ((jfew->k_f = XQueryFont(dpy, kanji_font_id)) == NULL) {
		return(-1);
	}
	if ((jfew->r_f = XQueryFont(dpy, kana_font_id)) == NULL) {
		return(-1);
	}
	jfew->g_f = NULL;

	/*      半角文字ＧＣ    */
	xgcv.foreground = jfew->fg;
	xgcv.background = jfew->bg;
	jfew->gch = XCreateGC(dpy, RootWindow(dpy, screen),
				(GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jfew->gch, jfew->h_f->fid);
	xgcv.foreground = jfew->bg;
	xgcv.background = jfew->fg;
	jfew->reversegch = XCreateGC(dpy, RootWindow(dpy, screen),
				(GCForeground | GCBackground),&xgcv);
	XSetFont (dpy, jfew->reversegch, jfew->h_f->fid);

	/*      全角文字ＧＣ    */
	xgcv.foreground = jfew->fg;
	xgcv.background = jfew->bg;
	jfew->gck = XCreateGC(dpy, RootWindow(dpy, screen),
				(GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jfew->gck, jfew->k_f->fid);
	xgcv.foreground = jfew->bg;
	xgcv.background = jfew->fg;
	jfew->reversegck = XCreateGC(dpy, RootWindow(dpy, screen),
				(GCForeground | GCBackground),&xgcv);
	XSetFont (dpy, jfew->reversegck, jfew->k_f->fid);

	xgcv.foreground = jfew->fg;
	xgcv.background = jfew->bg;
	jfew->gcr = XCreateGC(dpy, RootWindow(dpy, screen),
				(GCForeground | GCBackground ),&xgcv);
	XSetFont (dpy, jfew->gcr, jfew->r_f->fid);
	xgcv.foreground = jfew->bg;
	xgcv.background = jfew->fg;
	jfew->reversegcr = XCreateGC(dpy, RootWindow(dpy, screen),
				(GCForeground | GCBackground),&xgcv);
	XSetFont (dpy, jfew->reversegcr, jfew->r_f->fid);

	create_ichi();
	create_jutil();
	create_yes_no();
	send_ok();
/* MOTIF */
	read_motif_id();
/* MOTIF */
	XFlush(dpy);
	return(0);
}


send_ok()
{
    XEvent event;
    Atom select_id;

    if (!(select_id = XInternAtom(dpy, "DICSERVER_OK", True))) {
	select_id = XInternAtom(dpy, "DICSERVER_OK", False);
    }
    XSetSelectionOwner(dpy, select_id, jfew->jutil->w, 0L);
    event.type = ClientMessage;
    event.xclient.format = 32;
    event.xclient.window = jfew->jutil->w;
    event.xclient.data.l[0] = DIC_START;
    event.xclient.data.l[1] = jfew->jutil->w;
    XSendEvent(dpy, window_jfew,False, NoEventMask, &event);
    XFlush(dpy);
}

read_motif_id()
{
    register XJClient	*xjc;
    register int	cnt;

    xjc = jfew->xjclient;

    if ((motif_id = XInternAtom(dpy, "WM_PROTOCOLS", True))) {
	motif_id1 = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
	XChangeProperty(dpy, jfew->jutil->w, motif_id, XA_INTEGER, 32,
			PropModeReplace, &motif_id1, sizeof(Atom));
	XChangeProperty(dpy, jfew->ichi->w, motif_id, XA_INTEGER, 32,
			PropModeReplace, &motif_id1, sizeof(Atom));
	XChangeProperty(dpy, jfew->yes_no->w, motif_id, XA_INTEGER, 32,
			PropModeReplace, &motif_id1, sizeof(Atom));
    } else {
	motif_id = motif_id1 = 0;
    }
}
get_property()
{
    Atom atom_jfew;
    DIC_SERV_INFO *dic_info;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, byte_after, leftover;
    unsigned char *data;

    if ((dic_info = (DIC_SERV_INFO *)malloc(sizeof(DIC_SERV_INFO))) == NULL) {
	fprintf(stderr, "error malloc in get_property\n");
	return(-1);
    }
    if ((atom_jfew = XInternAtom (dpy, DICSERVER, True)) == (Atom)NULL) {
        free(dic_info);
	return(-1);
    }
    window_jfew = XGetSelectionOwner (dpy, atom_jfew);
    XSelectInput(dpy, window_jfew, StructureNotifyMask);
    XGetWindowProperty(dpy, DefaultRootWindow(dpy), atom_jfew, 0L, 1000000L, 
    			False,XA_STRING, &actual_type, &actual_format,
			&nitems, &leftover, &data);

    bcopy(data, dic_info, sizeof(DIC_SERV_INFO));
    strcpy(jserver_name_in_uumrc, dic_info->jserver_name);
    strcpy(uumrc_name, dic_info->uumrc_name);
    strcpy(uumkey_name_in_uumrc, dic_info->uumkey_name);
    strcpy(rkfile_name_in_uumrc, dic_info->rkfile_name);
    strcpy(cvt_fun_file, dic_info->cvtfun_name);
    strcpy(cvt_meta_file, dic_info->cvtmeta_name);
    strcpy(username, dic_info->user_name);
    ascii_font_id = dic_info->ascii_font_id;
    kanji_font_id = dic_info->kanji_font_id;
    kana_font_id = dic_info->kana_font_id;
    jfew->fg = dic_info->fore_ground;
    jfew->bg = dic_info->back_ground;
    free(dic_info);
    free(data);
    return(0);
}

get_bun_data()
{
    Atom atom_dicenv;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, byte_after, leftover;
    unsigned char *data, *data_sv;

    if ((atom_dicenv = XInternAtom (dpy, DICENV, True)) == (Atom)NULL) {
	return(-1);
    }
    XGetWindowProperty(dpy, DefaultRootWindow(dpy), atom_dicenv, 0L, 1000000L, 
    			False,XA_STRING, &actual_type, &actual_format,
			&nitems, &leftover, &data);

    data_sv = data;
    bcopy(data, &(bun_data_->env->env_id), sizeof(int));
    data += sizeof(int);
    bcopy(data, &(env_kankana_id), sizeof(int));
    /*
    bcopy(data + sizeof(int), bun_data_->env->js_id, sizeof(WNN_JSERVER_ID));
    bun_data_->env->js_id->js_dead_env_flg = 0;
    */
    free(data_sv);
    return(0);
}

get_touroku_data(buffer)
wchar *buffer;
{
    Atom atom_dicenv;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, byte_after, leftover;
    unsigned char *data, *data_sv;
    int ret;

    if ((atom_dicenv = XInternAtom (dpy, DICENV, True)) == (Atom)NULL) {
	return(-1);
    }
    XGetWindowProperty(dpy, DefaultRootWindow(dpy), atom_dicenv, 0L, 1000000L, 
    			False,XA_STRING, &actual_type, &actual_format,
			&nitems, &leftover, &data);

    data_sv = data;
    bcopy(data, &(bun_data_->env->env_id), sizeof(int));
    data += sizeof(int);
    bcopy(data, &(env_kankana_id), sizeof(int));
    data += sizeof(int);
    bcopy(data, &ret, sizeof(int));
    data += sizeof(int);
    bcopy(data, buffer, nitems - sizeof(int)*2);
    buffer[(nitems - sizeof(int)*3)/2] = 0;
    free(data_sv);
    return(ret);
}

clear_xjc(x)
XJClient *x;
{
	x->max_pos = 0;
	x->cursor_flag = 0;
	x->mark_flag = 0;
}

create_jutil()
{
    XSetWindowAttributes attributes;
    XJClient *xjc;
    XJClient *tmpxjc;
    Jutil *jutil;
    int screen;
    unsigned int width, height, bwidth;
    int cnt, t;

    if((jutil = (Jutil *)malloc(ret = sizeof(Jutil))) == NULL){
	fprintf(stderr, "error malloc in create_jutil\n");
	return(-1);
    }
    xjc = jfew->xjclient;
    jfew->jutil = jutil;
    screen = DefaultScreen(dpy);
    bwidth = 2;
    width = FontWidth * 100;
    height = FontHeight * 10;

    jutil->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
		0, DisplayHeight(dpy, screen) - height - bwidth * 2,
		width, height, bwidth, jfew->fg, jfew->bg);

    jutil->rk_mode_w = XCreateSimpleWindow(dpy, jutil->w,
		width - FontWidth * (MHL0 + 2), FontHeight/2,
		FontWidth * MHL0, FontHeight, 1, jfew->fg, jfew->bg);

    jutil->title_w = XCreateSimpleWindow(dpy, jutil->w,
		FontWidth, FontHeight/2,
		FontWidth * 8, FontHeight, 1, jfew->fg, jfew->bg);
    jutil->cancel_w = XCreateSimpleWindow(dpy, jutil->w,
		FontWidth * 12, FontHeight/2,
		FontWidth * 10, FontHeight, bwidth, jfew->fg, jfew->bg);
    jutil->exec_w = XCreateSimpleWindow(dpy, jutil->w,
		FontWidth * 24, FontHeight/2,
		FontWidth * 10, FontHeight, bwidth, jfew->fg, jfew->bg);

    XSelectInput(dpy, jutil->w, KeyPressMask | StructureNotifyMask);
    XSelectInput(dpy, jutil->cancel_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, jutil->exec_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);

    attributes.override_redirect = True;
    attributes.backing_store = Always;
    /*
    XChangeWindowAttributes (dpy, jutil->w,
		CWOverrideRedirect, &attributes);
    */
    XChangeWindowAttributes (dpy, jutil->title_w,
		CWBackingStore, &attributes);
    XChangeWindowAttributes (dpy, jutil->exec_w,
		CWBackingStore, &attributes);
    XChangeWindowAttributes (dpy, jutil->cancel_w,
		CWBackingStore, &attributes);
    XChangeWindowAttributes (dpy, jutil->rk_mode_w,
		CWBackingStore, &attributes);

    XDefineCursor(dpy, jutil->w, cursor3);
    XDefineCursor(dpy, jutil->cancel_w, cursor2);
    XDefineCursor(dpy, jutil->exec_w, cursor2);

    XStoreName(dpy, jutil->w, "Jutil");

    XMapWindow(dpy, jutil->rk_mode_w);
    XMapWindow(dpy, jutil->title_w);
    XMapWindow(dpy, jutil->cancel_w);
    XMapWindow(dpy, jutil->exec_w);

    for (t = 0; t < JCLIENTS; t++) {
	if((jutil->mes_jutil[t] = (Jclient *)malloc(sizeof(Jclient))) == NULL){
		fprintf(stderr, "error malloc in create_jutil\n");
		return(-1);
	}
	if((jutil->mes_jutil[t]->cl = tmpxjc = 
		(XJClient *)malloc(sizeof(XJClient))) == NULL){
		fprintf(stderr, "error malloc in create_jutil\n");
		return(-1);
	}

	jutil->mes_w[t] = XCreateSimpleWindow(dpy, jutil->w,
			FontWidth * 2,
			FontHeight * (2 + t * 3) + FontHeight/4,
			FontWidth * 100, FontHeight, 0,
			jfew->fg, jfew->bg);

	jutil->mes_jutil[t]->cl->w = XCreateSimpleWindow(dpy, jutil->w,
			FontWidth * 4,
			FontHeight * (3 + t * 3) + FontHeight/2,
			FontWidth * 90, FontHeight+FontHeight/2,
			0, jfew->fg, jfew->bg);

	XSelectInput(dpy, jutil->mes_w[t],
			ButtonPressMask | ButtonReleaseMask);
	tmpxjc->x = 0;
	tmpxjc->y = 0;
	tmpxjc->width = FontWidth * 90;
	tmpxjc->height = FontHeight + FontHeight/2;
	tmpxjc->p_width = tmpxjc->width;
	tmpxjc->p_height = FontHeight + FontHeight/2;
	tmpxjc->max_columns = 
		tmpxjc->p_width/FontWidth - 2;
	tmpxjc->bp = 1;
	if (create_jw(tmpxjc, 1, 0)) {
		return(-1);
	}
	XChangeWindowAttributes (dpy, jutil->mes_w[t],
			CWBackingStore, &attributes);
	XMapSubwindows(dpy, jutil->mes_jutil[t]->cl->w);
	XDefineCursor(dpy, jutil->mes_w[t], cursor2);
	XFlush(dpy);
    }
}

create_ichi() 
{
    XSetWindowAttributes	attributes;
    XGCValues	xgcv;
    XJClient	*xjc, *ichi_xjc;
    Ichiran		*ichi;
    int		screen;
    unsigned int	width, height, bwidth;
    int	cnt;

    xjc = jfew->xjclient;
    ichi = jfew->ichi;

    if((ichi->nyuuryoku_jutil = (Jclient *)malloc(sizeof(Jclient))) == NULL){
	fprintf(stderr, "error malloc\n");
	return(-1);
    }
    if((ichi->nyuuryoku_jutil->cl = (XJClient *)malloc(sizeof(XJClient))) == NULL){
	fprintf(stderr, "error malloc\n");
	return(-1);
    }
    if((ichi->keytable = (Keytable *)malloc(sizeof(Keytable))) == NULL){
	fprintf(stderr, "error malloc\n");
	return(-1);
    }
    ichi_xjc = ichi->nyuuryoku_jutil->cl;
    screen = DefaultScreen(dpy);

    width = 1;
    height = 1;
    bwidth = 2;

    xgcv.foreground = jfew->fg;
    xgcv.function = GXinvert;
    xgcv.plane_mask = XOR(jfew->fg, jfew->bg);
    ichi->invertgc = XCreateGC(dpy, RootWindow(dpy, screen),
				GCForeground | GCFunction | GCPlaneMask,&xgcv);

    ichi->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
			width, height, bwidth, jfew->fg, jfew->bg);
    ichi->comment_w = XCreateSimpleWindow(dpy, ichi->w, 0, FontWidth * 4,
			width, height, 1, jfew->fg, jfew->bg);
    ichi->w1 = XCreateSimpleWindow(dpy, ichi->w, 0, FontWidth * 4,
			width, height, 1, jfew->fg, jfew->bg);
    ichi->rk_mode_w = XCreateSimpleWindow(dpy, ichi->w,
			FontWidth * 1,
			FontHeight * 4 + FontHeight/2,
			FontWidth * MHL0, FontHeight,
			0, jfew->fg, jfew->bg);
    ichi->title_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth, FontHeight, 1, jfew->fg, jfew->bg);
    ichi->cancel_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi->subtitle_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth, FontHeight, 0, jfew->fg, jfew->bg);
    ichi->exec_title_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi->next_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi->back_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi_xjc->w = XCreateSimpleWindow(dpy, ichi->w,
			FontWidth * (MHL0 + 2),
			FontHeight * 4 + FontHeight/2,
			FontWidth * 60, FontHeight, 0,
			jfew->fg, jfew->bg);
    ichi->keytable->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi->keytable->key_w[0] = XCreateSimpleWindow(dpy, ichi->keytable->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi->keytable->key_w[1] = XCreateSimpleWindow(dpy, ichi->keytable->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);
    ichi->keytable->key_w[2] = XCreateSimpleWindow(dpy, ichi->keytable->w, 0, 0,
			FontWidth, FontHeight, bwidth,
			jfew->fg, jfew->bg);


    XDefineCursor(dpy, ichi->w, cursor3);
    XDefineCursor(dpy, ichi->cancel_w, cursor2);
    XDefineCursor(dpy, ichi->exec_title_w, cursor2);
    XDefineCursor(dpy, ichi->keytable->w, cursor3);
    XDefineCursor(dpy, ichi->keytable->key_w[0], cursor2);
    XDefineCursor(dpy, ichi->keytable->key_w[1], cursor2);
    XDefineCursor(dpy, ichi->keytable->key_w[2], cursor2);

    XStoreName(dpy, ichi->w, "ichiran");

    {
	ichi_xjc->x = 0;
	ichi_xjc->y = 0;
	ichi_xjc->width = FontWidth * 60;
	ichi_xjc->height = FontHeight;
	ichi_xjc->p_width = ichi_xjc->width;
	ichi_xjc->p_height = FontHeight;
	ichi_xjc->max_columns = 
			ichi_xjc->p_width/FontWidth - 2;
	ichi_xjc->bp = 0;
	if (create_jw(ichi_xjc, 1, 0)) {
		return(-1);
	}
    }
    XSelectInput(dpy, ichi->w, KeyPressMask);
    XSelectInput(dpy, ichi->cancel_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, ichi->exec_title_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, ichi->next_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, ichi->back_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, ichi->keytable->w, KeyPressMask);
    XSelectInput(dpy, ichi->keytable->key_w[0],
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, ichi->keytable->key_w[1],
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, ichi->keytable->key_w[2],
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);

	attributes.override_redirect = True;
/*
	XChangeWindowAttributes (dpy, ichi->w, CWOverrideRedirect, &attributes);
*/
	XChangeWindowAttributes (dpy, ichi->keytable->w,
				CWOverrideRedirect, &attributes);

	attributes.backing_store = Always;
	XChangeWindowAttributes (dpy, ichi->title_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->cancel_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->comment_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->w1,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->subtitle_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->exec_title_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->back_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->next_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->rk_mode_w,
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->keytable->key_w[0],
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->keytable->key_w[1],
			CWBackingStore, &attributes);
	XChangeWindowAttributes (dpy, ichi->keytable->key_w[2],
			CWBackingStore, &attributes);
/*
	XSelectInput(dpy, ichi_xjc->w1, ExposureMask);
*/
	XMapWindow(dpy, ichi->title_w);
	XMapWindow(dpy, ichi->cancel_w);
	XFlush(dpy);
}

create_yes_no()
{
    XSetWindowAttributes	attributes;
    XJClient	*xjc = jfew->xjclient;
    YesOrNo *yes_no = jfew->yes_no;
    int		screen;
    unsigned int	width, height, bwidth;
    int	cnt;

    screen = DefaultScreen(dpy);

    width = 1;
    height = 1;
    bwidth = 2;

    yes_no->w = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
			width, height, bwidth, jfew->fg, jfew->bg);
    yes_no->title_w = XCreateSimpleWindow(dpy, yes_no->w, 0,0,
			width, height, 0, jfew->fg, jfew->bg);
    yes_no->yes_w = XCreateSimpleWindow(dpy, yes_no->w, FontWidth * 2,
			FontHeight * 2 + FontHeight/2 - IN_BORDER,
			FontWidth * 6 + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2, bwidth,
			jfew->fg, jfew->bg);
    yes_no->no_w = XCreateSimpleWindow(dpy, yes_no->w, FontWidth * 10,
			FontHeight * 2 + FontHeight/2 - IN_BORDER,
			FontWidth * 6 + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2, bwidth,
			jfew->fg, jfew->bg);

    XDefineCursor(dpy, yes_no->w, cursor3);
    XDefineCursor(dpy, yes_no->yes_w, cursor2);
    XDefineCursor(dpy, yes_no->no_w, cursor2);
    XSelectInput(dpy, yes_no->yes_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);
    XSelectInput(dpy, yes_no->no_w,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask);

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
    /*
    XMapWindow(dpy, yes_no->yes_w);
    XMapWindow(dpy, yes_no->no_w);
    */
    XFlush(dpy);
}


/*********************************************************************
	終了 ルーチン
			from err_end(), do_end()
*********************************************************************/
xw_end()
{
    if(dpy) {
	free(jfew->xjclient);
	free(jfew);
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
	perror("dicserver");
	epilogue();
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
    int code;
    char strbuf[STRBUFSIZE];	
    register int ret;
    int	 keycode;
    int	 nbytes;
    int	 ck;
    extern int two_byte_first0;

    for(;;){
	XNextEvent(dpy, &event);
	switch (event.type) {
	    case KeyPress:
		nbytes = XLookupString (&event, strbuf, STRBUFSIZE,
					&keycode, &compose_status);
		if (event.xkey.state & Mod1Mask) { /* 全面キー */
		    if ((ck = cvtmeta(keycode)) == -1) {
			break;
		    } else {
			*buff++ = ck;
			return(1);
		    }
		}
		if ((keycode & 0xff00) == 0xff00) { /* ファンクションキー */
		    if ((keycode >= XK_Multi_key && keycode <= XK_Eisuu_Lock) ||
			(keycode >= XK_Home && keycode <= XK_F35)) {
			if ((ck = cvtfun(keycode)) == -1) {
			    break;
			} else {
			    *buff++ = ck;
			    return(1);
			}
		    } else {
			if (nbytes > 0) {
			    strncpy(buff, strbuf, nbytes);
			    buff += nbytes;
			    return(nbytes);
			} else {
			    break;
			}
		    }
		}
		if (event.xkey.state & ControlMask) { /* コントロールキー */
		    if (*strbuf >= 0x20) {
			break;
		    } else if (*strbuf == 0x0) {
			*buff++ = *strbuf;
			return(1);
		    } else if (*strbuf <= 0x1f) {
			*buff++ = *strbuf;
			return(1);
		    }
		}
		if (( keycode >= 0x4a0) && (keycode <= 0x4df)){ /* 仮名キー */
		    *buff++ = SS2;
#ifdef	X11R3
		    *buff = (char)(keycode & 0xff);
		    return(2);
#else	/* X11R3 */
		    strncpy(buff, strbuf, nbytes);
		    return(nbytes + 1);
#endif	/* X11R3 */
		} else if ((keycode < 0x100) && (keycode > 0)){
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
		if (work) break;
		two_byte_first0 = 0;
		switch(event.xclient.data.l[0]) {
		    case DIC_ADD:	
			work = 1;
			jishoadd();
			work = 0;
			break;
		    case DIC_KENSAKU:
			work = 1;
			kensaku();
			work = 0;
			break;
		    case DIC_ICHIRAN:
			work = 1;
			select_one_dict9();
			work = 0;
			break;
		    case DIC_PARAM:
			work = 1;
			paramchg();
			work = 0;
			break;
		    case DIC_INFO:
			work = 1;
			dicinfoout();
			work = 0;
			break;
		    case DIC_FZK:
			work = 1;
			fuzoku_set();
			work = 0;
			break;
		    case DIC_TOUROKU:
			work = 1;
			touroku();
			work = 0;
			break;
		    default:
			break;
		}

	    case Expose:
		xw_expose(&event);
		break;
	    case ButtonPress:
		button_x = event.xbutton.x_root;
		button_y = event.xbutton.y_root;
		if(xw_buttonpress(&event) == 0) {
		    *buff = 0xd;
		    return(1);
		}
		break;
	    case DestroyNotify:
		terminate_handler();
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
	if (ev->window == jfew->ichi->w || ev->window == jfew->jutil->w) {
		select_return = -2;
		return(0);
	}
	return(1);
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
    int	w_width, lines;
    XWindowAttributes	attr;
    XSetWindowAttributes	ch_attr;
    XEvent			event;
    Window	root;
    Window	parent;
    Window	*child;
    unsigned int	nchild;
    register int	cnt;

	if((xjc->buf= (int *)malloc(sizeof(int) * (maxchg * 2 + 1))) == NULL){
		fprintf(stderr, "error malloc\n");
		return(-1);
	}
	for (cnt = (maxchg * 2); cnt >= 0; cnt--) {
		*(xjc->buf + cnt) = 0;
	}
	clear_xjc(xjc);

	height = jfew->h_f->ascent + jfew->h_f->descent;
#ifdef DEBUG
	border_width = 1;
#else
	border_width = 0;
#endif
	xjc->firstx = 0;
	xjc->firsty = (xjc->height - FontHeight)/2;
	xjc->maxcolumns = maxchg * 2;
		
	xjc->wp = XCreateSimpleWindow(dpy, xjc->w, -(xjc->bp), -(xjc->bp),
			xjc->p_width, xjc->height, xjc->bp, jfew->fg, jfew->bg);

	xjc->wp1 = XCreateSimpleWindow(dpy, xjc->wp, 
			FontWidth * (mhl0 + 1), xjc->firsty,
			FontWidth * xjc->max_columns, height,
			border_width, jfew->fg, jfew->bg);

	xjc->wnf = XCreateSimpleWindow(dpy, xjc->wp,
			FontWidth * mhl0, xjc->firsty,
			FontWidth, height,
			border_width, jfew->fg, jfew->bg);

	xjc->wnb = XCreateSimpleWindow(dpy, xjc->wp,
			FontWidth * (xjc->max_columns + mhl0 + 1),
			xjc->firsty,
			FontWidth, height,
			border_width, jfew->fg, jfew->bg);

	xjc->wnn = XCreateSimpleWindow(dpy, xjc->wp,
			FontWidth * (xjc->max_columns + mhl0),
			xjc->firsty,
			FontWidth, height,
			border_width, jfew->fg, jfew->bg);

	xjc->w1 = XCreateSimpleWindow(dpy, xjc->wp1,
			0, 0,
			FontWidth * (xjc->maxcolumns + 1), height,
			border_width, jfew->fg, jfew->bg);

	xjc->vst = 0;
	xjc->note_fore = 0;
	xjc->note_back = 0;

	ch_attr.save_under = True;
	ch_attr.backing_store = Always;
	XChangeWindowAttributes(dpy, xjc->wp, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->w1, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wnf, CWBackingStore, &ch_attr);
	XChangeWindowAttributes(dpy, xjc->wnb, CWBackingStore, &ch_attr);

	XMapWindow(dpy, xjc->wp);
	XSelectInput(dpy, xjc->wp, KeyPressMask);
	return(0);
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
	XMoveWindow(dpy, xjc->w1, -(xjc->vst * FontWidth), 0);
	XFlush(dpy);
}

check_move(xjc,kan)
register XJClient	*xjc;
int	kan;
{
    int	currentcol_tmp;

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
	JWM_note_delete_fore();
	xjc->note_fore = 0;
    }
    if (xjc->max_pos > (xjc->vst + xjc->max_columns)) {
	JWM_note_back();
	xjc->note_back = 1;
	if ((check_kan(xjc->buf, xjc->vst + xjc->max_columns - 1)) == 1)
		XMapWindow(dpy, xjc->wnn);
	else
		XUnmapWindow(dpy, xjc->wnn);
    } else {
	JWM_note_delete_back();
	xjc->note_back = 0;
	XUnmapWindow(dpy, xjc->wnn);
    }
}

JWM_note_null()
{
	register XJClient	*xjc;
	int	x, y;

	xjc = cur_jutil->cl;

	x = xjc->x + (xjc->maxcolumns - 1) * FontWidth;
	y = xjc->y;
	XMoveWindow(dpy, xjc->wnn, x, y);
	XMapWindow(dpy, xjc->wnn);
}

check_scroll()
{
	XJClient	*xjc;
	int		kan;

	xjc = cur_jutil->cl;
	kan = check_kan(xjc->buf, xjc->currentcol);
	if (xjc->currentcol < xjc->vst
	|| (xjc->currentcol - xjc->vst - kan) >= xjc->max_columns) {
		if (kan == 1)
			check_move(xjc, 1);
		else
			check_move(xjc, 0);
	}
}

#undef	malloc

char *Salloc(size)
unsigned size;
{
	return((char *)calloc(1, size));
}
