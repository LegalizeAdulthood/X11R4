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
 *	Original Source: ./lib/Wnn/uum/prologue.c
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

/* Standard Interface
 *    Initialize Routine
 */

#include "xwnmo.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <pwd.h>
#include "jllib.h"
#include "commonhd.h"
#include "config.h"
#include "buffer.h"
#include "rk_spclval.h"
#ifdef SYSVR2
#ifndef	X11R3
#ifdef UX386
#   include <X11/Xos.h>
#else
#   include <fcntl.h>
#   include <string.h>
#endif
#endif	/* X11R3 */
#endif
#if !defined(X11R3) && !defined(UX386)
#include <sys/types.h>
#include <sys/file.h>
#endif	/* X11R3 */
#ifdef BSD42
#   include <strings.h>
#endif
#include "sdefine.h"
#include "sheader.h"

extern char     *getenv();
extern FILE	*fopen();




/** romkan のイニシャライズ */
open_romkan()
{
    char name[1024];
    int rk_option_flg;
    extern int keyin();

    if (*rkfile_name_in_uumrc != '\0') {
	strcpy(name, rkfile_name_in_uumrc);
    }else {
	    strcpy(name , RKFILE);
    }
    rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete? 0 : RK_SIMPLD);

    return(romkan_init5(name,rubout_code,keyin,NULL,rk_option_flg) == 0 ?
    	   0 : -1);
				/* romaji-kana henkan junbi */
}


/** uumrc ファイルのオープン */
int
get_uumrc_name(str)
char *str;
{
    if(envrc_name_in_uumrc[0] == '\0') {
	strcpy(str, ENVRCFILE);
    }else{
	strcpy(str, envrc_name_in_uumrc);
    }
    return(1);
}

/* uum で必要なエリアをアロケートする。*/
allocate_areas()
{
    char *malloc();
    char *area_start;
    char *area_pter;

    int k;
    int total_size;
    int len1 = maxchg * sizeof(wchar);

    total_size = len1 * 4 + maxbunsetsu * sizeof(int) * 3;

    if ((area_start = malloc(total_size)) == NULL){
	fprintf(stderr,"初期化でmalloc不能。\n");
	return(-1);
    }

    area_pter = area_start;
    return_buf = (wchar *)area_pter;  /* mojiretsu -> return_buf */
    area_pter += len1;
    input_buffer = (wchar *)area_pter;
    area_pter += len1;
    kill_buffer = (wchar *)area_pter;
    area_pter += len1;
    remember_buf = (wchar *)area_pter;	/* ADD KURI */
    *remember_buf = 0;		 	/* ADD KURI */
    area_pter += len1;			/* ADD KURI */
    bunsetsu  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    bunsetsuend  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    touroku_bnst  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    for (k = 0; k < maxbunsetsu; k++) {
	bunsetsu[k] = 0;
	bunsetsuend[k] = 0;
	touroku_bnst[k] = 0;
    }

    return(0);
}

free_areas()
{
    free(return_buf);

}

/* henkan initialize */
/** uum のイニシャライズ */
init_uum()   
{
    char	*p;

    uumrc_get_entries();

#ifndef	X11
    if(*(p = convkey_name_in_uumrc) == '\0') p = NULL;
    if(convert_key_setup(p, (verbose_option != 0)) == -1) return(-1);
#else	X11
    if (init_xcvtkey() == -1) return(-1);
#endif	!X11
    
    if (init_key_table() == -1) {
	return(-1);
    }

    if(open_romkan() == -1){
	return(-1);
    }

    if (allocate_areas() == -1) {
	return(-1);
    }

    /* connect_jserver(0); */	/* chamge KURI */

    return(0);
}

static puts1(s)
char *s;
{
    print_msg_getc(s);
}
static puts2(s)
char *s;
{
    print_msg_getc(s);
}
static yes_or_no_init(s)
char *s;
{
    return(yes_or_no(s));
}

/** Connecting to jserver */
int
connect_jserver(first)
int first;
{
    register int dn;
    register int kasz, klsz, kjsz;
    char *malloc();
    /*char uumrc_name[PATHNAMELEN];	ADD KURI */
    extern int yes_or_no(), put2();	/* ADD KURI */
    extern int save_env_id;

    /*get_uumrc_name(uumrc_name);	 ADD KURI */
	
    bun_data_ = jl_open(username, jserver_name_in_uumrc, uumrc_name,
			   yes_or_no_init, puts2, WNN_TIMEOUT);

    if (bun_data_ != NULL && bun_data_->env != NULL) {
	if(kanakan_sticky)
	    jl_env_sticky(bun_data_);
    }
    save_env_id = bun_data_->env->env_id;
    return(0);
}


int get_login_dir(dir_name)     /* kore de iinoka check shuru. */
char *dir_name;
{
 char *s;
 s=getenv("HOME");
    if (s == NULL){
	fprintf(stderr , "環境変数「HOME」が異常です。\n");
	return(-1);
    } else {
	strcpy(dir_name , s);
	return(1);
    }
}

#ifdef	X11
init_xcvtkey()
{
    if (!*cvt_fun_file)
	strcpy(cvt_fun_file, CVTFUNFILE);
    if (cvt_fun_setup(cvt_fun_file) == -1) {
	fprintf(stderr, "File \"%s\" can't open.", cvt_fun_file);
	return(-1);
    }
    if (!*cvt_meta_file)
	strcpy(cvt_meta_file, CVTMETAFILE);
    if(cvt_meta_setup(cvt_meta_file) == -1) {
	fprintf(stderr, "File \"%s\" can't open.", cvt_meta_file);
	return(-1);
    }
    return(0);
}
#endif	X11
