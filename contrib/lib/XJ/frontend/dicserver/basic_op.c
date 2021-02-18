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
 *	Original Source: ./lib/Wnn/uum/basic_op.c
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
/* 
buffer�λȤ����Ȥ�buffer�����Ƥ��ɤΤ褦�˲��̤�ɽ������뤫�Ȥ�
�ξܤ��������ϡ�buffer.h�����ϡ�screen.c�򻲾Ȥ��Ʋ�������
���Υե�����ϡ�baffer �����椹�뤿��δ��ܴؿ��ǡ�
�桼�������󥿡��ե���������Ͽ��������¾�����Ϥ˶��̤���褦�ʴؿ���
������Ƥ��ޤ���
*/
#include <stdio.h>
#include "jllib.h"
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"

/** buffer��Ȥäơ���̾ʸ��������Ϥ��ޤ���
buflen�ϥХåե�����Ĺ����mode��ȿž�������ξ���
buffer����˽��ʸ�����������֤��Ʋ�������
msg�ϡ���̾���ϹԤ���Ƭ��ɽ����å�����ʸ(w_char�Ǥ��뤳�Ȥ����)
���˥������뤹����ˤϡ����Υ�å���������˥������뤷�ޤ�
����ϡ�buffer��Ȥ����٥�Τ������ϥ롼����Ǥ���
���˹��٤ʻ���򤷤����ͤϡ�buffer_in���Ѥ��Ʋ�������
*/

redraw_nisemono()
{
    t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
    check_scroll();
}

/** buffer��Ȥäơ���̾ʸ��������Ϥ��ޤ���
kana_in_w_char_msg�Ȥΰ㤤�ϡ�
msg����char�Ǥ��뤳�Ȥȡ����˥������뤹����ˤϡ����Υ�å������������
�������뤹�뤳�ȡ�
*/
int  kana_in(mode, buffer, buflen)
int mode;
w_char *buffer;
int buflen;
{
  struct buf c_b1, *c_btmp;

  c_btmp = c_b;
  c_b = &c_b1; 

  c_b->buffer = buffer;
  c_b->buf_msg = "";

  c_b->hanten = mode;
  c_b->start_col = 0;
  c_b->maxlen = Strlen(buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen;
  c_b->t_m_start = -2;
  set_screen_vars_default();
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = NULL;
  kill_buffer_offset = 0;

  init_screen();
  if(buffer_in() == -1){
      select_return = -1;
      c_b = c_btmp;
      return(-1);
  }
  if (select_return == -2) {
      select_return = -1;
      c_b = c_btmp;
      return(-1);
  }
  c_b->buffer[c_b->maxlen] = 0;
  c_b = c_btmp;
  return(0);
}

int kana_in_w_char_msg(mode , buffer , buflen)
int mode;
w_char *buffer;
int buflen;
{
  static struct buf c_b1, *c_btmp;
  c_btmp = c_b;
  c_b = &c_b1; 
  if((c_b->buffer =(w_char *)malloc(buflen * 2)) == NULL){
      print_msg_getc("Malloc�˼��Ԥ��ޤ�����(ǡ��)");
      return(-1);
  }
  c_b->t_m_start = 0;
  Strcpy(c_b->buffer, buffer);
  c_b->buf_msg = "";

  c_b->hanten = mode;
  c_b->start_col = 0;
  c_b->maxlen = Strlen(c_b->buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen;
  set_screen_vars_default();
  c_b->redraw_fun = redraw_nisemono;
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  kill_buffer_offset = 0;

  init_screen();
  if(buffer_in() == -1){
      free(c_b->buffer);
      c_b = c_btmp;
      return(-1);
  }
  free(c_b->buffer);
  Strncpy(buffer , c_b->buffer, c_b->maxlen);
  buffer[c_b->maxlen] = 0;
  c_b = c_btmp;
  return(0);
}


/** �������Ϥ�Ԥ����������椹�뤿��δ��ܥ롼����
�����Х��ѿ�c_b�λؤ���(struct buf)�ˡ����ʽ������򤷤Ƥ���
�����Ƥ�ǲ�������
ʸ����ϡ�c_b->buffer�����äƽ�λ���ޤ���
c_b->maxlen��ʸ�����Ĺ��������ޤ���
*/
int buffer_in()
{
  int c;
  int ret;
  int romkan;
  int ignored = 0;
  int not_redrawtmp = not_redraw;

  extern int kakutei();		/* ADD KURI */
  
  not_redraw = 0;
  while(1){
    c = ((romkan = romkan_getc()) & 0x0000ffff);

    if(isSPCL(romkan)){
	if(romkan == REDRAW){
		(*c_b->redraw_fun)();
		continue;
	} else if(romkan == CHMSIG) { /* mode changed */
		disp_mode();
		continue;
	} else if(romkan == NISEBP) { /* error */
		ring_bell();
		continue;
	} else {
		continue; /* if (romkan == EOLTTR) */
	}
    }
    if (ignored){
      if (isNISE(romkan)) {
	if (c == rubout_code) {
	  --ignored;
	} else {
	  ++ignored;
	}
      } else {
	ignored = 0;
      }
	continue;
    }
	/* ��ʪ�Υ���ȥ��륳���ɤϲ���ɽ�����ʤ��ΤǸ�����rubout
	�Υ����ɤ�̵�뤹��ɬ�פ����롣ignored��̵�뤹�٤�
	rubout�θĿ����ݻ����롣*/
    if (isNISE(romkan) && (ESCAPE_CHAR(c) ||  NOT_NORMAL_CHAR(c)) &&
	c != rubout_code ){
      ++ignored;
	continue;
    }

    if(c < TBL_SIZE &&  c_b->key_table[c]){
	/* KURI
	if (romkan_clear_tbl[c])
	*/
	if (c_b->rk_clear_tbl[c])
	    romkan_clear();
	ret = (*c_b->key_table[c])(c,romkan);
    }else if(c >= 0x80 && c < 0xff){
		ret = 0;
		ring_bell();
    }else if(!(ESCAPE_CHAR(c)) || c == 0x09){	/* TAB is not ESCAPE char */
      if(c_b->maxlen < c_b->buflen){
	if(c_b->key_in_fun){
		ret = (*c_b->key_in_fun)(c , romkan);
	} else {
		ret = insert_char(c);
		/* change KURI
		t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,1);
		*/
		t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
#ifdef	X11
		check_scroll();
		kk_cursor_normal();
#endif	X11
	}
      }else{
	romkan_clear();
	ret = 0;
      }

    }else{
      if(c_b->ctrl_code_fun){
	ret = (*c_b->ctrl_code_fun)(c,romkan);
      }else{
	ret = 0;
	ring_bell();
      }
    }
    if(ret == 1){
      break;
    }else if(ret == -1){
	not_redraw = not_redrawtmp;
	return(-1);
    }
  }
  not_redraw = not_redrawtmp;
  return(0);
}

int
t_rubout(c, romkan)
int c, romkan;
{

  if(c_b->t_c_p != 0){
      backward();
      t_delete_char();
  }
  return(0);
}


int
delete_char1()
{
  
  if(c_b->t_c_p < c_b->maxlen){	
    Strncpy(c_b->buffer + c_b->t_c_p , c_b->buffer + c_b->t_c_p + 1,
	    c_b->maxlen - c_b->t_c_p - 1);
    c_b->maxlen -= 1;
  }
}

t_delete_char()
{
    delete_char1();
#ifdef	X11
    kk_cursor_invisible();
#endif	X11
    t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
#ifdef	X11
    check_scroll();
    kk_cursor_normal();
#endif	X11
    return(0);
}

int t_kill()
{
  kill_buffer_offset = min(c_b->maxlen - c_b->t_c_p , maxchg);
  Strncpy(kill_buffer , c_b->buffer + c_b->t_c_p , kill_buffer_offset);
  c_b->maxlen = c_b->t_c_p;
  t_redraw_move(c_b->t_c_p ,c_b->t_c_p, c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

int t_ret()
{
  return(1);
}

int
t_quit()
{
  return(-1);
}


int 
c_top()
{
  if(c_b->t_m_start < 0){
    t_move(0);
  }else{
    t_move(c_b->t_m_start);
  }
  return(0);
}

int 
c_end()
{
    t_move(c_b->maxlen);
    return(0);
}

int t_jmp_backward()
{
  int k;
  
  for(k = 1 ; k < touroku_bnst_cnt; k++){
    if(touroku_bnst[k] >= c_b->t_c_p){
      if(touroku_bnst[k - 1] >= c_b->t_m_start){
	  t_move(touroku_bnst[k - 1]);
      }else{
	  c_top();
      }
      return(0);
    }
  }
  backward();
  return(0);
}

int t_jmp_forward()
{
  int k;
  
  for(k = 1 ; k < touroku_bnst_cnt; k++){
    if(touroku_bnst[k] > c_b->t_c_p){
	t_move(touroku_bnst[k ]);
	return(0);
    }
  }
  forward();
  return(0);
}


int
forward()
{
  if(c_b->t_c_p < c_b->maxlen){
      t_move(c_b->t_c_p + 1);
  }
  return(0);
}

int
backward()
{
  if(c_b->t_c_p >= 1 && c_b->t_c_p > c_b->t_m_start){
      t_move(c_b->t_c_p - 1);
  }
  return(0);
}

int
insert_char(c)
{
    if(c_b->maxlen < c_b->buflen){
	Strncpy(c_b->buffer + c_b->t_c_p + 1, c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
	c_b->maxlen += 1;
	c_b->buffer[c_b->t_c_p] = c;
    }
    return(0);
}
int
insert_string(string, len)
w_char *string;
int len;
{
  if(c_b->maxlen + len <= c_b->buflen){
    Strncpy(c_b->buffer + c_b->t_c_p + len, c_b->buffer + c_b->t_c_p , c_b->maxlen - c_b->t_c_p);
    Strncpy(c_b->buffer + c_b->t_c_p, string , len);
    c_b->maxlen += len;
  }
}

/** vlen��duplicate��Ŭ���Ȼפ�����֤����ꤷ�ޤ���*/
set_screen_vars_default()
{
#ifndef	X11
  c_b->vlen = maxlength * conv_lines - c_b->start_col - 3 - disp_mode_length ;
  c_b->vst = 0;
  c_b->duplicate = max((c_b->vlen + 3) >> 2 , 2);
#else	X11
  c_b->vlen = maxchg * 2;
  c_b->vst = 0;
  c_b->duplicate = 0;
#endif	!X11
}

quote()
{
    int c;
    c = romkan_next();
    if (empty_modep() && c_b->key_in_fun)
	return ((*c_b->key_in_fun)(c, c));
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    romkan_clear();
    return(0);
}

