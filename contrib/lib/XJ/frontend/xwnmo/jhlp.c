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
 *	Original Source: ./lib/Wnn/uum/jhlp.c
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

#include "xwnmo.h"
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "config.h"

#include <stdio.h>
#include <signal.h>
#ifndef	X11R3
#include <fcntl.h>
#endif	/* X11R3 */
#include <pwd.h>
struct passwd *getpwuid();

#ifdef	BSD42
#	include <sgtty.h>
#endif	/* BSD42 */

#ifdef SYSVR2
#	include "l_term.h"
#endif /* SYSVR2 */

#ifdef UX386
#define SIGCHLD SIGCLD
#endif

extern char *optarg;
extern int optind;

extern char *getenv();

static void save_signals();
static int dic_server_pid;
extern DIC_SERV_INFO *dic_info;

/** メイン */
main(argc, argv)
int argc;
char **argv;
{
    extern terminate_handler();
    char *name;
    char *p;
    prgname = argv[0];

    if (init_clients() == -1) {
	exit(-1);
    }
    strcpy(username, getpwuid(getuid())->pw_name);
    if((name = getenv(WNN_USERNAME_ENV)) != NULL){
	strcpy(username, name);
    }
    if((name = getenv(WNN_JSERVER_ENV)) == NULL) {
	name = "";
    }
    strcpy(jserver_name_in_uumrc, name);

#ifndef	X11
    parse_options(argc, argv);
#else	X11
    X_options(argc, argv);
#endif	X11

    save_signals();

#ifndef	X11
#ifdef	BSD42
    if (getTermData() == -1) {
	err("Sorry. Something is wrong with termcap, maybe.\r\n");
	exit(21);
    }
#endif	/* BDS42 */
#ifdef	SYSVR2
    if (openTermData() == -1) {
	err("Sorry. Something is wrong with terminfo, maybe.\r\n");
	exit(21);
    }
#endif	/* SYSVR2 */
#endif	!X11

    get_rubout();

#ifndef NOTFORK
    if (fork()) {
	signal(SIGTERM, exit);
	signal(SIGCHLD, exit);
	pause();
    }
#endif	NOTFORK

    switch (init_uum()) {	/* initialize of kana-kanji henkan */
    case -1:
	terminate_handler();
	break;
    case -2:
	epilogue();
	do_end();
	break;
    }

#ifdef	X11
    if(create_jfew() == -1)
	exit(-1);
    jfew->xjclient->w_c = c_c;
    client_mem[0] = jfew->xjclient;
    strcpy(dic_info->user_name, username);
    connect_jserver(0);
#endif	X11
    init_clients1();

    if (!jl_isconnect(bun_data_)) {
      if (jserver_name_in_uumrc && *jserver_name_in_uumrc){
	print_msg_getc(" ｊｓｅｒｖｅｒ(at %s)と接続できませんでした。",
		  jserver_name_in_uumrc);
      } else {
	print_msg_getc(" ｊｓｅｒｖｅｒと接続できませんでした。");
      }
    }
/*
    _disp_window = 1;
*/

      close(0);
      close(1);

#ifndef NOTFORK
    /* End of initialization, kill parent */
    kill(getppid(), SIGTERM);
#endif /* !NOTFORK */

    clear_c_b();

#ifdef	SYSVR2
	setpgrp();
#endif	/* SYSVR2 */
#ifdef	BSD42
	setpgrp(0, 0);
#endif	/* BSD42 */
    do_main();
}

#ifdef	X11
dicserver_start()
{
    strcpy(dic_info->jserver_name, jserver_name_in_uumrc);
    set_dicserver();
    signal(SIGCHLD, SIG_IGN);

#ifndef	NOTFORK
    if ((dic_server_pid = fork()) == 0) {
	close(dpy->fd);
	start_dicserver();
	exit(0);
    }
#endif	NOTFORK
}

dicserver_kill()
{
	kill(dic_server_pid, SIGTERM);
}
#endif	/* X11 */

/*
  each options handling functions
 */

#ifndef	X11
int	disp_at_cursor = 1;	/* 表示する位置 0: 画面の下、1:カーソル位置 */
/* text object でもカーソル位置に出さない */
static int do_t_opt()
{
    defined_by_option |= OPT_DISP_AT_CURSOR;
    disp_at_cursor = 0;
    return(0);
}

static int do_T_opt()
{
    defined_by_option |= OPT_DISP_AT_CURSOR;
    disp_at_cursor = 2;
    return(0);
}
#endif	!X11

static int do_h_opt()
{
    henkan_off_def = 1;
    defined_by_option |= OPT_WAKING_UP_MODE;

    return 0;
}

static int do_H_opt()
{
    henkan_off_def = 0;
    defined_by_option |= OPT_WAKING_UP_MODE;

    return 0;
}

static int do_k_opt()
{
    strcpy(uumkey_name_in_uumrc, optarg);
    if (*uumkey_name_in_uumrc == '\0') {
	return -1;
    }
    defined_by_option |= OPT_WNNKEY;

    return 0;
}

#ifndef	X11
static int do_c_opt()
{
    strcpy(convkey_name_in_uumrc, optarg);
    if (*convkey_name_in_uumrc == '\0') {
	return -1;
    }
    defined_by_option |= OPT_CONVKEY;

    return 0;
}
#endif	!X11

static int do_r_opt()
{
    strcpy(rkfile_name_in_uumrc, optarg);
    if (*rkfile_name_in_uumrc == '\0') {
	return -1;
    }
    defined_by_option |= OPT_RKFILE;

    return 0;
}

static int do_D_opt()
{
    strcpy(jserver_name_in_uumrc, optarg);
    if (*jserver_name_in_uumrc == '\0') {
	return -1;
    }

    return 0;
}

static int do_n_opt()
{
    strcpy(username, optarg);
    if (*username == '\0') {
	return -1;
    }

    return 0;
}

static int do_v_opt()
{
    defined_by_option |= OPT_VERBOSE;
    return 0;
}

#ifdef	X11
static int  do_F_opt()
{
    strcpy(cvt_fun_file, optarg);
    if (*cvt_fun_file == '\0') {
	return -1;
    }
    defined_by_option |= OPT_FUNFILE;

    return 0;
}

static int  do_M_opt()
{
    strcpy(cvt_meta_file, optarg);
    if (*cvt_meta_file == '\0') {
	return -1;
    }
    defined_by_option |= OPT_MTFILE;

    return 0;
}
#endif	X11

static int (*do_opt[])() = {
#ifndef	X11
    do_t_opt,			/* 't' : not use henkan line cursor position */
    do_T_opt,			/* 'T' : use henkan line cursor position */
#endif	!X11
    do_h_opt,			/* 'h' : waking_up_in_henkan_mode */
    do_H_opt,			/* 'H' : waking_up_no_henkan_mode */
    do_k_opt,			/* 'k' : specify uumkey file */
#ifndef	X11
    do_c_opt,			/* 'c' : specify convert_key file */
#endif	!X11
    do_r_opt,			/* 'r' : specify romkan mode file */
    do_D_opt,			/* 'D' : specify hostname of jserver */
    do_n_opt,			/* 'n' : specify username for jserver */
    do_v_opt,			/* 'v' : verbose */
#ifdef	X11
    do_F_opt,			/* 'F' : specify cvt_fun_key file */
    do_M_opt			/* 'M' : specify cvt_meta_key file */
#endif	X11
  };

static parse_options(argc, argv)
int argc;
char **argv;
{
  register int c;
#ifndef	X11
  register char *ostr = WNMO_OPTIONS;
#else	X11
  register char *ostr = XWNMO_OPTIONS;
#endif	!X11
  register char *p;
#ifndef	SX8870
  extern char *index();
#else
  extern char *strchr();
#endif
  
#ifndef	X11
  while ((c = getopt(argc, argv, WNMO_GETOPTSTR)) != EOF) {
#else	X11
  while ((c = getopt(argc, argv, XWNMO_GETOPTSTR)) != EOF) {
#endif	!X11
#ifndef	SX8870
    if ((p = index(ostr, c)) == NULL || (*do_opt[p - ostr])() < 0) {
#else
    if ((p = strchr(ostr, c)) == NULL || (*do_opt[p - ostr])() < 0) {
#endif
#ifndef	X11
      usage(ostr);
#else	X11
      usage();
#endif	!X11
    }
  }
}

#ifdef X11
extern	char	fore_color[32];
extern	char	back_color[32];
extern	char	boder_color[32];
extern	char	ascii_font[256];
extern	char	kanji_font[256];
extern	char	display_name[256];
X_options(argc, argv)
int	argc;
char	**argv;
{
	char	sargv[20][256];
	int	sargc = 1;
	int	i;
	extern int iconic_start;

	for (i = 1; i < argc; i++) {
		strcpy(sargv[i], argv[i]);
	}
	for (i = 1; i < argc; i++) {
		if (!strcmp(sargv[i], "-fg") || !strcmp(sargv[i], "-foreground")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(fore_color, sargv[i]);
			defined_by_option |= OPT_FOREGROUND;
		} else if (!strcmp(sargv[i], "-bg") || !strcmp(sargv[i], "-background")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(back_color, sargv[i]);
			defined_by_option |= OPT_BACKGROUND;
		} else if (!strcmp(sargv[i], "-bd") || !strcmp(sargv[i], "boderground")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(boder_color, sargv[i]);
			defined_by_option |= OPT_BORDERGROUND;
		} else if (!strcmp(sargv[i], "-fn")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(ascii_font, sargv[i]);
			defined_by_option |= OPT_ASCFONT;
		} else if (!strcmp(sargv[i], "-fk")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(kanji_font, sargv[i]);
			defined_by_option |= OPT_KANJIFONT;
		} else if (!strcmp(sargv[i], "-fr")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(kana_font, sargv[i]);
			defined_by_option |= OPT_KANAFONT;
		} else if (!strcmp(sargv[i], "-d") || !strcmp(sargv[i], "-display")) {
			if (++i >= argc)
				usage();
			if ( sargv[i][0] == '-')
				usage();
			strcpy(display_name, sargv[i]);
			defined_by_option |= OPT_DISPLAYNAME;
		} else if (!strcmp(sargv[i], "-ic") || !strcmp(sargv[i], "-iconc")) {
			iconic_start = 1;
		} else {
			strcpy(argv[sargc], sargv[i]);
			sargc = sargc + 1;
		}
	}
	parse_options(sargc, argv);
}
#endif X11


/* rubout_code の取り出し */
#define UNDEF_STTY 255

#ifdef BSD42
get_rubout()
{
	struct sgttyb savetmio;
	int	fd;

	if ((fd = open("/dev/console",O_WRONLY)) == -1) {
		rubout_code = RUBOUT;		/* BackSpase */
		return;
	}
	if (ioctl(fd, TIOCGETP, &savetmio) < 0) {
		close(fd);
		return(-1);
	}
	if (savetmio.sg_erase == UNDEF_STTY) {
		rubout_code = RUBOUT;
	} else {
		rubout_code = savetmio.sg_erase;
	}
	close(fd);
}
#endif /* BSD42 */

#ifdef SYSVR2
get_rubout()
{
	struct termio tmio;
	int	fd;

	if ((fd = open("/dev/console",O_WRONLY)) == -1) {
		rubout_code = RUBOUT;		/* BackSpase */
		return;
	}
	if (ioctl(fd, TCGETA, &tmio) < 0) {
		rubout_code = RUBOUT;		/* BackSpase */
		close(fd);
		return;
    	}
	if (tmio.c_cc[VERASE] == UNDEF_STTY) {
		rubout_code = RUBOUT;
	} else {
		rubout_code = tmio.c_cc[VERASE];
	}
	close(fd);
	return(0);
}
#endif /* SYSVR2 */

/** signal SIGTERM を受けた時の処理をする。*/
terminate_handler()
{
    do_end();
}

/** メインループ */

do_main()
{
    register int ml;
    int	c;

    if (henkan_off_flag == 0) {
	disp_mode();
    }
#ifdef	X11
    else {
	xw_display_title();
    }
#endif	X11

    for (;;) {
	if (rk_buf_len() <= 0) {
	    c = keyin0();
	    ungetkeyin0(c);

	    if (henkan_off_flag == 1) {
		henkan_off();
		continue;
	    }
	    c = conv_keyin();
	    if (rk_push_back(c) < 0)
		continue;
	}


	ml = kk();
	if (ml < 0)
	    continue;

      make_history(return_buf, ml);
#ifndef	X11
      gmw_write(return_buf, ml);
#else	X11
      xw_write(return_buf, ml);
#endif	!X11
    }
}

/** convert_key nomi okonau key-in function */
int conv_keyin()
{
    int keyin0();

#ifndef	X11
    return keyin1(keyin0);
#else	X11
    return(keyin0());
#endif	!X11
}

/** キー入力関数 1 */
int keyin()
{

    return(conv_keyin());

}

static int ungetbuf = -1;
/** キー入力関数 2 */
int keyin0()
{
    static char buf[BUFSIZ];
    static char *bufend = buf;
    static char *bufstart = buf;
    int  n;

    if (ungetbuf != -1) {
	n = ungetbuf & 0xff;
	ungetbuf = -1;
	return(n);
    }

    if (bufstart < bufend) {
        return (*bufstart++);
    }
    for (;;) {
#ifdef	GMW2
        if ((n = gmw_read(buf)) > 0) {
#endif	GMW2
#ifdef	X11
        if ((n = xw_read(buf)) > 0) {
#endif	X11
            bufstart = buf;
            bufend = buf + n;
            return ((int)*bufstart++);
        }
    }
}

ungetkeyin0(c)
char c;
{
    ungetbuf = c;
}

/** エラーだよ。さようなら。 */

err(s)
char *s;
{
    puts(s);
    fclose(stdout);
    fclose(stderr);
    fclose(stdin);
    do_end();
}

/** 立つ鳥後を濁さず 終わりの処理 */
do_end()
{
#ifndef	X11
	gmw_end();
#else	X11
	xw_end();
#endif	!X11
	epilogue();
#ifdef	DICSERV
	dicserver_kill();
#endif	/* DICSERV */
	exit(0);
}

#ifndef X11
static usage()
{
	fprintf(stderr, "usage: %s [-hHkc:r:D:n:]\n", prgname);
	exit(0);
}
#else X11
static usage()
{
/*
	err_end("使用方法: %s [-display hostname:n] [-fg fore_color] [-bg back_color]\n                 [-bd border_coler] [-k uumkey-file] [-F convert-function-key-file]\n                 [-M convert-meta-key-file] [-r romkan-mode-file]\n                 [-D server-name] [-n user-name] ", "usage: %s [-display hostname:n] [-fg fore_color] [-bg back_color]\n                 [-bd border_coler][-k uumkey-file] [-F convert-function-key-file]\n                 [-M convert-meta-key-file] [-r romkan-mode-file]\n                 [-D server-name] [-n user-name]", prgname);
*/
	fprintf(stderr, "usage: %s [-display hostname:n] [-D server-name]\n             [-fg fore_color] [-bg back_color] [-bd border_coler]\n             [-fn ascii-font] [-fk kanji-font] [-fr kana-font]\n             [-k uumkey-file] [-F convert-function-key-file]\n             [-M convert-meta-key-file] [-r romkan-mode-file]\n             [-n user-name] [-h] [-H] \n", prgname);
	exit(0);
}
#endif !X11


/*
  signal settings
 */

static void save_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
#ifndef	NOTFORK
    signal(SIGINT,  SIG_IGN);
#endif	NOTFORK
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, terminate_handler);
    signal(SIGCHLD, SIG_IGN);
/*
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
#endif SIGTSTP
*/
}
