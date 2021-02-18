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
 *	Original Source: ./lib/Wnn/uum/select_ele.c
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

/* 一つの要素をユーザーに取り出してもらうためのルーチン */

#define DD_MAX 512

#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "buffer.h"
#include "jslib.h"
#include "jllib.h"

#define printh(mm)\
  if(state == 0){\
    printf("%c.",((mm - dd[cc]) > 9)? mm - dd[cc] - 10  + 'A':\
				      mm - dd[cc] + '0');\
    printf("%s", data[mm]);\
  }else{\
    printf("%c.",((mm > 9)? mm - 10  + 'A':mm + '0'));\
    printf("%s", data[mm]);}
		    

#define kouho_len(x) (strlen(x) + 4)

/*
#define WIDTH (maxlength - disp_mode_length - 6)
*/
#define LEND c_b->vlen + 1
#define WIDTH (LEND - 7)

static char **data; /* mojiretu data area*/
static int dd[512]; /* gamen no sentou to naru youso wo motte iru */
static int dd_size; /* dd no size */
static char *msg;
static int msg_len;
static int cc; /* ima no gamen */
static int mm;/* hyouji mojiretu */
static int state; /* choose whether to number all or not */
				/* 0: line by line */
				/* 1: all */
static int kosuu; /* kosuu of elements */
extern hilited_ji_item;
extern hilited_item;
extern max_line;

xw_select_one_element(c,kosuu1,init,title,mode, key_tbl)
char **c; /*表示文字列の配列のポインタ */
int kosuu1;
int init;
char *title;
int mode;
int (**key_tbl)();
{
  int oldmm;
  static int d;
  int c1;
  int ret;
  static struct wnmo_client *c_c_sv = 0;
  static int select_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (select_step == 0) {
	kosuu = kosuu1;
	if (init_ichiran(c, kosuu1,init,title,"","",0,mode) == -1) {
	    ring_bell();
	    return(-1);
	}
	c_c_sv = c_c;
	select_step++;
	return(BUFFER_IN_CONT);
    }
#ifdef	WNMO
    c1 = romkan_getc() ;
#else	WNMO
    c1 = romkan_next() ;
#endif	WNMO
    if (select_return == -1) {
    /*
        if (! ONEBYTE_CHAR(c1)) {
	    return(BUFFER_IN_CONT);
	}
    */
        d = change_decimal(c1);
    } else if (select_return == -2) {
        end_ichiran();
	select_return = -1;
	c_c_sv = 0;
	select_step = 0;
	romkan_clear();
	return(-1);
    } else {
	end_ichiran();
	d = select_return;
	select_return = -1;
    }
    if(d >= 0 && d < kosuu){
	romkan_clear();
	end_ichiran();
	c_c_sv = 0;
	select_step = 0;
	return(d);
    } 
    if((c1 < 256) && (key_tbl[c1] != NULL)){
	if((ret = (*key_tbl[c1])()) == 1){
	    d = find_hilite();
            end_ichiran();
	    c_c_sv = 0;
	    select_step = 0;
	    romkan_clear();
            return(d);
	}else if(ret == -1){
	    end_ichiran();
	    c_c_sv = 0;
	    select_step = 0;
	    romkan_clear();
	    return(-1);
	} else if(ret == -3){
      	    ring_bell();
	}
    }
    romkan_clear();
    return(BUFFER_IN_CONT);
}

xw_select_one_jikouho(c,kosuu1, init,title, mode, key_tbl)
char **c; /*表示文字列の配列のポインタ */
int kosuu1;
int init;
char *title;
int mode;
int (**key_tbl)();
{
  int ret;
  static int d, c1;
  static int c_p = 0;
  static struct wnmo_client *c_c_sv = 0;
  static int select_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (select_step == 0) {
	kosuu = kosuu1;
	if (init_ichiran(c, kosuu1, init,title,"","",0, mode) == -1) {
	    ring_bell();
	    return(-1);
	}
	c_c_sv = c_c;
	select_step++;
	return(BUFFER_IN_CONT);
    }
    if (select_step == 1) {
	romkan_clear();
	c1 = rk_getc();
	if (select_return == -2) {
	    end_ichiran();
	    select_return = -1;
	    c_c_sv = 0;
	    select_step = 0;
	    romkan_clear();
	    return(-1);
	} else if (select_return != -1) {
	    d = find_ji_hilite();
	    end_ichiran();
	    select_return = -1;
	    c_c_sv = 0;
	    select_step = 0;
	    romkan_clear();
	    return(d);
	}else if ((c1 < 256) && (key_tbl[c1] != NULL)){
	    if((ret = (*key_tbl[c1])()) == 1){
		d = find_ji_hilite();
		end_ichiran();
		c_c_sv = 0;
		select_step = 0;
		romkan_clear();
		return(d);
	    } else if (ret == -1) {
		end_ichiran();
		c_c_sv = 0;
		select_step = 0;
		romkan_clear();
		return(-1);
	    } else if (ret == -3) {
		ring_bell();
		return(BUFFER_IN_CONT);
	    } else {
		return(BUFFER_IN_CONT);
	    }
  	} else {
	    ring_bell();
	    return(BUFFER_IN_CONT);
	}
    }
}


static 
display_all()
{
    redraw_ichi_w(NULL);
}

static int
change_decimal(c1)
{
  if(c1 >= '0' && c1 <= '9'){
    return(c1 - '0');
  }
  if(c1 >= 'A' && c1 <= 'Z'){
    return(c1 - 'A' + 10);
  }
  if(c1 >= 'a' && c1 <= 'z'){
    return(c1 - 'a' + 10);
  }
  return(-1);
}

forward_select()
{
    xw_forward_select();
    return(0);
}

backward_select()
{
    xw_backward_select();
    return(0);
}

lineend_select()
{
  xw_lineend_select();
  return(0);
}

linestart_select()
{
  xw_linestart_select();
  return(0);
}

select_select()
{
  return(1); /* return mm from upper function */
}

quit_select()
{
  return(-1);
}

previous_select()
{
    xw_previous_select();
  return(0);
}

next_select()
{
    xw_next_select();
  return(0);
}

redraw_select()
{
  display_all();
  return(0);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/

