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

#include "rk_spclval.h"

#define	BUFSIZ	1024

static int	for_romkan_buf[BUFSIZ];
static int	*start_of_r_buf = for_romkan_buf;
static int	*end_of_r_buf = for_romkan_buf + BUFSIZ;
static int	*in_of_r_buf = for_romkan_buf;
static int	*out_of_r_buf = for_romkan_buf;
static int	count = 0;
static int	save_of_key;

int
rk_getc()
{
    int	ret;

    ret = *out_of_r_buf;
    if (count > 0) {
	if (++out_of_r_buf == end_of_r_buf)
	    out_of_r_buf = start_of_r_buf;
	count--;
	return (ret);
    }
    return (-1);
}

rk_getkey()
{
    int	ret;

    ret = save_of_key;
    if (count > 0) {
    	for(;count > 0;) {
	    if (++out_of_r_buf == end_of_r_buf)
		out_of_r_buf = start_of_r_buf;
	    count--;
    	}
	return(ret);
    }
    return (-1);
}

rk_buf_len()
{
    return (count);
}

rk_push_back(c)
int	c;
{
    static two_byte_first = 0;
    extern int *romkan_henkan();
    int *output;

    if (c == -1)
	c = LTREOF;
    else
	c &= 0xff;

    if (two_byte_first == 0) {
	if ((c > 0xa0 && c < 0xff) || c == 0x8e) {
	    two_byte_first = c << 8;
	    return (-1);
	}
    }
    c |= two_byte_first;
    two_byte_first = 0;
    save_of_key = c;

    output = romkan_henkan(c);

    for ( ; *output != EOLTTR ; output++, count++) {
	*in_of_r_buf = *output;
	if (++in_of_r_buf == end_of_r_buf)
	    in_of_r_buf = start_of_r_buf;
    }
    return (0);
}
