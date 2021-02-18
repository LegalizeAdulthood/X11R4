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
 *	Original Source: ./lib/Wnn/uum/history.c
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
#include "jslib.h"

static struct history {
	wchar *hbuffer;
	struct history *previous;
	struct history *next;
	int	num;
};

static struct history *beginning_of_history; /* ヒストリの先頭 */
static struct history *end_of_history; /* ヒストリの最後 */
static struct history *add_history; /* 追加位置 */
static struct history *current_history = NULL; /* index */
static int	wchar_holding = 0;
static int	history_cunt;

init_history()
{
    char *malloc();
    int size1;
    char *area_pter;

    /*
    register wchar *hbuf;
    */
    register struct history *hist, *pre, *end;

    if ((history_cunt = max_history) < 1) {
	history_cunt = 0;
	return(0);
    }
    size1 = history_cunt * sizeof(struct history );
    if ((area_pter = malloc(size1)) == NULL) {
	history_cunt = 0;
	wnn_errorno = WNN_MALLOC_ERR;
	return(-1);
    }
    hist = beginning_of_history = end_of_history = add_history =
	(struct history *)area_pter;
    /*
    hbuf = (wchar * )(area_pter + size1);
    */
    pre = end = hist + (history_cunt - 1);
    do {
	hist->hbuffer = NULL;
	pre->next = hist;
	hist->previous = pre;
	hist->num = 0;
	pre = hist++;
    } while (hist <= end);
    return(0);
}


make_history(wstr, n)
register wchar *wstr;
register int	n;
{
    if (!history_cunt)
	return(0);
    if (n <= 0 || n > maxchg || *wstr == NULL) 
	return(-1);

    if (n == 1 && NORMAL_CHAR(*wstr)) {
	if (!wchar_holding) {
	    wchar_holding = 1;
	    set_up_history();
	    end_of_history->num = 0;
	}
	if (end_of_history->hbuffer == NULL)
	    end_of_history->hbuffer = (wchar *)malloc(sizeof(wchar));
	else
	    end_of_history->hbuffer = (wchar *)realloc(end_of_history->hbuffer,
				     (end_of_history->num + 1) * sizeof(wchar));
	Strncpy(end_of_history->hbuffer + end_of_history->num, wstr, 1);
	if (++(end_of_history->num) >= maxchg)
		wchar_holding = 0;
	return(0);
    }
    if (wchar_holding)
	wchar_holding = 0;
    if (n == 1 && !(KANJI_CHAR(*wstr)))
	/* コントロールコードは、ヒストリには入れない */
	return(0);
    if (end_of_history->num == n &&
	    Strncmp(end_of_history->hbuffer, wstr, n) == 0) {
	/* 同じものは、いや */
	current_history = NULL;
	return(0);
    }
    set_up_history();
    if (end_of_history->hbuffer != NULL)
	free(end_of_history->hbuffer);
    end_of_history->hbuffer = (wchar *)malloc(n * sizeof(wchar));
    Strncpy(end_of_history->hbuffer, wstr, n);
    end_of_history->num = n;

    return(0);
}


static
set_up_history()
{
    if (add_history == beginning_of_history &&
	    end_of_history != beginning_of_history)
	beginning_of_history = beginning_of_history->next;
    end_of_history = add_history;
    add_history = add_history->next;
    current_history = NULL;
}


get_current_history(wbuf)
register wchar *wbuf;
{
    if (!history_cunt)
	return(0);
    if (wchar_holding)
	wchar_holding = 0;
    if (current_history == NULL)
	current_history = end_of_history;
    Strncpy(wbuf, current_history->hbuffer, current_history->num);
    return(current_history->num);
}


/* add 86/8/18 確定後に登録する漢字を取り出す */
get_end_of_history(wbuf)
register wchar	*wbuf;
{
    if (!history_cunt) {
	*wbuf = 0;
    } else {
    if (wchar_holding)
	    wchar_holding = 0;
	Strncpy(wbuf, end_of_history->hbuffer, end_of_history->num);
	*(wbuf + end_of_history->num) = 0;
    }
}


previous_history1(buffer)
register wchar *buffer;
{
    if (!history_cunt || current_history == beginning_of_history)
	return(0); /* do nothing */

    if (current_history != NULL)
	current_history = current_history->previous;
    return(get_current_history(buffer));
}


next_history1(buffer)
register wchar *buffer;
{
    if (!history_cunt || current_history == end_of_history || current_history == NULL)
	return(0);

    current_history = current_history->next;
    return(get_current_history(buffer));
}


