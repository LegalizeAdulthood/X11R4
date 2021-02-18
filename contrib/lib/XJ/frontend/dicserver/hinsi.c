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
 *	Original Source: ./lib/Wnn/uum/hinsi.c
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

#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "jllib.h"

/*#define ROOT "… ªÏ/" */
#define ROOT "/"
wchar root[sizeof(ROOT) * 2];

char *maeni = "¡∞§ÀÃ·§Î";

#define MAXHINSI 256

char *bunpou_msg = "… ªÏ¡™¬Ú";
int save_k[5];
int save_k_p;
char *bunpou_title;
int save_num[10];
int save_num_p;
char *save_t[10];
int select_ret;

static wchar *
bunpou_search(node)
wchar *node;
{
    int cnt;
    int k;
    char *hbuf[MAXHINSI];
    wchar tmp[WNN_HINSI_NAME_LEN];
    char buf[1024];		/* iikagen */
    char *c;
    wchar **whbuf;
    wchar *ret;

    if(node == NULL) return(NULL);
    cnt = jl_hinsi_list(bun_data_, -1, node, &whbuf);
    if(cnt == -1){
	errorkeyin();
	return(NULL);
    }
    if(cnt == 0) return(node);
    for(k = 0, c = buf ; k < cnt ; k++){
	hbuf[k] = c;
	sStrcpy(c, whbuf[k]);
	c += strlen(c) + 1;
    }
    hbuf[cnt] = maeni;
 TOP:
    k = xw_select_one_element(hbuf, cnt + 1, 0,bunpou_title,
				0, NULL,NULL,NULL,NULL);
    if(k == -1)return(NULL);
    if(k == -3) {
	select_ret = -3;
	return(NULL);
    }
    if(strcmp(hbuf[k], maeni) == 0) {
	JWMline_clear(save_k[--save_k_p]);
	bunpou_title = save_t[save_num_p];
	return((w_char *)maeni);
    }
    Sstrcpy(tmp, hbuf[k]);
    save_k[save_k_p++] = cur_jutil->cl->currentcol;
    xw_jutil_write_msg(hbuf[k]);
    save_t[save_num_p] = bunpou_title;
    bunpou_title = hbuf[k];
    ret = bunpou_search(tmp);
    if(ret == NULL) return(NULL);
    if(k == -3) {
	select_ret = -3;
	return(NULL);
    }
    if(strcmp((char *)ret, maeni) == 0) goto TOP;
    return(ret);
}


hinsi_in()
{
    wchar *a;

    Sstrcpy(root, ROOT);
    not_redraw = 1;
    bunpou_title = bunpou_msg;
    save_k_p = 0;
    save_num_p = 0;
    save_t[0] = bunpou_title;

    if((a = bunpou_search(root)) == NULL ||
       strcmp((char *)a, maeni) == 0) {
	not_redraw = 0;
	return(-1);
    }
    not_redraw = 0;
    return(jl_hinsi_number(bun_data_, a));
}
