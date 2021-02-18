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

extern char *optarg;
extern int optind;

extern char *getenv();

static void save_signals();

/** メイン */
main(argc, argv)
int argc;
char **argv;
{
    extern terminate_handler(), exit();
    char *name;
    char *p;
    prgname = argv[0];

    strcpy(display_name, argv[1]);

    save_signals();

    get_rubout();


#ifdef	X11
    maxchg = MAXCHG;
    maxlength = MAXCHG * 2;
    if(create_jfew() == -1)
	exit(-1);
#endif	X11
    switch (init_uum()) {	/* initialize of kana-kanji henkan */
    case -1:
	terminate_handler();
	break;
    case -2:
	epilogue();
	do_end();
	break;
    }
    connect_jserver(0);

    if (!jl_isconnect(bun_data_)) {
      if (jserver_name_in_uumrc && *jserver_name_in_uumrc){
	print_msg_getc(" ｊｓｅｒｖｅｒ(at %s)と接続できませんでした。",
		  jserver_name_in_uumrc);
      } else {
	print_msg_getc(" ｊｓｅｒｖｅｒと接続できませんでした。");
      }
    }

#ifdef	SYSVR2
	setpgrp();
#endif
#ifdef	BSD42
	setpgrp(0, 0);
#endif

    do_main();
}

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
    char buf[BUFSIZ];

    for (;;) {
#ifdef	X11
      xw_read(buf);
#endif	X11
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
	err_end("使用方法: %s [-display hostname:n] [-fg fore_color] [-bg back_color]\n                 [-bd border_coler] [-k wnnkey-file] [-F convert-function-key-file]\n                 [-M convert-meta-key-file] [-r romkan-mode-file]\n                 [-D server-name] [-n user-name] ", "usage: %s [-display hostname:n] [-fg fore_color] [-bg back_color]\n                 [-bd border_coler][-k wnnkey-file] [-F convert-function-key-file]\n                 [-M convert-meta-key-file] [-r romkan-mode-file]\n                 [-D server-name] [-n user-name]", prgname);
*/
	fprintf(stderr, "usage: %s [-display hostname:n] [-fg fore_color] [-bg back_color]\n                 [-bd border_coler][-k wnnkey-file] [-F convert-function-key-file]\n                 [-M convert-meta-key-file] [-r romkan-mode-file]\n                 [-D server-name] [-n user-name]\n", prgname);
	exit(0);
}
#endif !X11


/*
  signal settings
 */

static void save_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP,  terminate_handler);
#ifndef	NOTFORK
    signal(SIGINT,  SIG_IGN);
#endif	NOTFORK
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, terminate_handler);
    signal(SIGCHLD, SIG_IGN);
}
