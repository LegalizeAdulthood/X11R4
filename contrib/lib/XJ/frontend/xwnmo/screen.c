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
 *	Original Source: ./lib/Wnn/uum/screen.c
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
#include "buffer.h"

/*
 ���Υե�����ϡ�����������ʬ�ȤΥ��󥿡��ե�������Ԥ��ؿ���
������Ƥ��롣�����δؿ����Ѥ����ˡ����̤����椷�ƤϤʤ�ʤ���
���Υե����뤫��extern ����Ƥ���ؿ��򼨤���
�ʤ������̤�����ϡ�c_b->buffer ����ǤΥ���ǥå������Ѥ���
�Ԥ��롣����c_b->buffer ����Τɤ���ʬ�����̤˽Ф���Ƥ��뤫�ϰռ����ʤ�
�Ƥ褤��
t_move(x) : ��������� x �����Ф�����ɥ���ɬ��(ȿž��������Τ���)
����ʬ�⾡��˹ͤ��ƹԤä����롣
t_redraw_move(x , start , end,clr_l) :buffer ����ǡ�start ���� end �ޤǤ�
�Хåե������Ƥ��Ѥ���줿���ˡ����������x �����Ф��Τ��Ѥ��롣
print_buf_msg(msg) :��å�������ʬ��ɽ�����롣
disp_mode():romkan �Υ⡼�ɤ�ɽ�����롣
disp_mode_line():romkan �Υ⡼�ɤ�ɽ�����롣
t_print_l(): ���̤Υ�ɥ����롣

t_throw(): ������������Ф�
init_screen():���̴ط�(vst)�򥤥˥���饤����������ɽ����Ԥ���
*/

/* ʸ���β��̾�Ǥ��礭�����֤���*/

static int col = 0; /** startichi����β��̾�Υ���������� */
static int oldmaxcol = 0; /** redraw sita tokini doko made mae ni kaita data ga nokotteiruka wo simesu */

#define CHAR_BYTE 2

#define w_putchar_move(x) {col += (char_len(x)); w_putchar(x);}
#define putchar_move(x) {col += 1; putchar_norm(x);}
#define putchar_move1(x) {col += 1; putchar1(x);}

#define throw0(x)	throw_c((x) + c_b->start_col)
/*
#define throw(x) {int tmptmp = (x);if(cs == 0 || col != tmptmp){throw0((col = tmptmp) + 1);}}
*/
#define throw(x) throw0((col = (x)) + 1)

/* ���������col �����Ф��Τˤϡ�throw��
�Ѥ��롣 */
#define MARJIN 1 /* �������뤬��ü���餳������ν�ˤ���С����β��̤˹Ԥ�*/

/* vst �򥻥åȤ�ľ���ơ���ɥ�����*/
static int t_redraw_one_line()
{
#ifndef	X11
  if(c_b->vst){
    t_cont_line_note();
  }else{
    t_cont_line_note_delete();
  }
#endif	!X11
  t_print_line(c_b->vst, c_b->maxlen,1);
}

init_screen(){
#ifndef	X11
  int tmp;

  oldmaxcol = 0;  
  tmp = cur_ichi(c_b->t_c_p , 0);
  if((tmp >= c_b->vlen - c_b->duplicate) &&(tmp % (c_b->vlen - c_b->duplicate)) < c_b->duplicate - 1){
    c_b->vst = find_character_on_that_col((tmp / (c_b->vlen - c_b->duplicate) - 1)*(c_b->vlen - c_b->duplicate), 0);
  }else{
    c_b->vst = find_character_on_that_col((tmp / (c_b->vlen - c_b->duplicate))*(c_b->vlen - c_b->duplicate), 0);
  }
#else	X11
  c_b->vst = 0;
#endif	X11
  if(c_b->maxlen || *c_b->buf_msg)t_print_l();
}

/* hituyouga areba vst wo settosinaosite, settosinaosita tokiniha 1 wo kaesu */
#ifndef	X11
int check_vst()
{
    int k = -1;

  if((c_b->t_c_p < c_b->vst) ||
     (cur_ichi(c_b->t_c_p , c_b->vst) >= (c_b->vlen - MARJIN )) ||
     (cur_ichi(c_b->t_m_start , c_b->vst) > (c_b->vlen - MARJIN ))){
      k = set_vst();
  }
  if(k == -1){
      return(0);
  }else{
      return(1);
  }
}
#endif	!X11

int
t_redraw_move(x , start , end,clr_l)
int clr_l;
{
    c_b->t_c_p = x;
#ifndef	X11
    if(check_vst()){/* screen moved */
	t_redraw_one_line();
    }else{
	t_print_line(start, end,clr_l);
    }
#else	X11
    t_print_line(start, end,clr_l);
#endif	!X11
    throw(cur_ichi(c_b->t_c_p , c_b->vst));
    flush();
    return(0);
}

int
t_move(x)
{
  int old_cp = c_b->t_c_p;

  if(((c_b->hanten >> 2) & 0x3) != ((c_b->hanten >> 4) & 0x3)){
    t_redraw_move(x , min(old_cp, x), max(old_cp, x),0);
    return(0);
  }
  if(x >= c_b->maxlen)x = c_b->maxlen;
  c_b->t_c_p = x;
#ifndef	X11
  if(check_vst()){
      t_redraw_one_line();
  }
#endif	!X11
  throw(cur_ichi(c_b->t_c_p , c_b->vst));
  flush();
  return(0);
}

t_print_l()
{
  clr_line_all();
  if (*(c_b->buf_msg))
  print_buf_msg(c_b->buf_msg);
  t_redraw_one_line();
  throw(cur_ichi(c_b->t_c_p , c_b->vst));
  flush();
  return(0);
}

/*
#define set_screen_reverse(X,Y){if(c_b->hanten & X){h_r_on();}else{ h_r_off();}if(c_b->hanten & Y){u_s_on();}else{ u_s_off();} flush();}
#define reset_screen_reverse(X,Y) {flushw_buf();}
*/

#define set_screen_reverse(X,Y){set_hanten_ul(c_b->hanten & X, c_b->hanten & Y);}
#define reset_screen_reverse(X,Y){flushw_buf(); if(c_b->hanten & X)h_r_off();if(c_b->hanten & Y)u_s_off(); flush();}
/* add by KUWA */
#define set_screen_bold(X){flushw_buf(); set_bold(c_b->hanten & X);}
#define reset_screen_bold(X){flushw_buf(); reset_bold(c_b->hanten & X);flush();}
/* add by KUWA */

/* st must be bigger than vst */


/* ���Υե�����ǰ��ֽ��פʴؿ���
c_b->buffer ����ǡ�st ���� end �ޤǤǲ��̤˸���Ƥ����(vst�����)��
ɽ����ľ�������������⤹�롣
��С����ξ����ϡ�c_b->hanten�ˤ�äƷ����롣
*/

static
t_print_line(st , end, clr_l) 
int st , end, clr_l;
{
  register int k;
  register int col1;
  register int end_of_line = maxlength - disp_mode_length - 1 - c_b->start_col ;
  int tmp;

  int mst = min(c_b->t_m_start , c_b->t_c_p);
  int mend = max(c_b->t_m_start , c_b->t_c_p);
  
  int bst = c_b->t_b_st;	/* add by KUWA */
  int bend = c_b->t_b_end;	/* add by KUWA */
  int bold = 0;			/* add by KUWA */

  if(end > c_b->maxlen)end = c_b->maxlen;
  col1 = cur_ichi(st , c_b->vst);
  if(col1 == -1){
    col1 = 0;
    k = st = c_b->vst;
  }else{
    k = st;
  }

  for(;end_of_line <= col1 ; end_of_line += maxlength);

  throw(col1);
  if(mst >= 0){
    if(st < mst){
      set_screen_reverse(0x01,0x02);
      for(k = st ; k <mst ; k++){
	/* add by KUWA */
	if(bold == 0 && k >= bst && k < bend) {
	  set_screen_bold(0x40);
	  bold = 1;
	} else if (bold && (k < bst || k >= bend)) {
	  reset_screen_bold(0x40);
	  set_screen_reverse(0x01,0x02);
	  bold = 0;
	}
	/* add by KUWA */
	if(k >= end){
	  reset_screen_reverse(0x01,0x02);
	  reset_screen_bold(0x40); /* add by KUWA */
	  bold = 0;
	  goto normal_out;
	}
	tmp =  char_len(c_b->buffer[k]);
	if(tmp + col > c_b->vlen){
	  reset_screen_reverse(0x01,0x02);
	  reset_screen_bold(0x40); /* add by KUWA */
	  bold = 0;
	  goto end_syori;
	}
	w_putchar_move(c_b->buffer[k]);
	if(col > end_of_line - CHAR_BYTE){
	    flushw_buf();
	    if(col  < end_of_line){
		reset_screen_reverse(0x01,0x02);
	  	reset_screen_bold(0x40); /* add by KUWA */
	  	bold = 0;
		putchar_move(' ');
		set_screen_reverse(0x01,0x02);
		if(k >= bst && k < bend) {	/* add by KUWA */
		  set_screen_bold(0x40);
		  bold = 1;
		}
	    }
	    throw(end_of_line);
	    end_of_line += maxlength;
	}

      }
      reset_screen_reverse(0x01,0x02);
      reset_screen_bold(0x40); /* add by KUWA */
      bold = 0;
    }
    if(k < mend){
      set_screen_reverse(0x04,0x08);
      for(; k < mend ; k++){
	/* add by KUWA */
	if(bold == 0 && k >= bst && k < bend) {
	  set_screen_bold(0x40);
	  bold = 1;
	} else if (bold && (k < bst || k >= bend)) {
	  reset_screen_bold(0x40);
	  set_screen_reverse(0x04,0x08);
	  bold = 0;
	}
	/* add by KUWA */
	if(k >= end){
	  reset_screen_reverse(0x04,0x08);
	  reset_screen_bold(0x40); /* add by KUWA */
	  bold = 0;
	  goto normal_out;
	}
	tmp = char_len(c_b->buffer[k]);
	if(col + tmp > c_b->vlen ){
	  reset_screen_reverse(0x04,0x08);
	  reset_screen_bold(0x40); /* add by KUWA */
	  bold = 0;
	  goto end_syori;
	}
	w_putchar_move(c_b->buffer[k]);
	if(col > end_of_line - CHAR_BYTE){
	    flushw_buf();
	    if(col < end_of_line){
		reset_screen_reverse(0x04,0x08);
	  	reset_screen_bold(0x40); /* add by KUWA */
	  	bold = 0;
		putchar_move(' ');
		set_screen_reverse(0x04,0x08);
		if(k >= bst && k < bend) {
		  set_screen_bold(0x40);
		  bold = 1;
		}
	    }
	    throw(end_of_line);
	    end_of_line += maxlength;
	}
      }
      reset_screen_reverse(0x04,0x08);
      reset_screen_bold(0x40); /* add by KUWA */
      bold = 0;
    }
  }
  if(k < c_b->maxlen && k < end){
      set_screen_reverse(0x10,0x20);
      for(; k <c_b->maxlen ; k++){
	  /* add by KUWA */
	  if(bold == 0 && k >= bst && k < bend) {
	    set_screen_bold(0x40);
	    bold = 1;
	  } else if (bold && (k < bst || k >= bend)) {
	    reset_screen_bold(0x40);
	    set_screen_reverse(0x10,0x20);
	    bold = 0;
	  }
	  /* add by KUWA */
	  if(k >= end){
	      reset_screen_reverse(0x10,0x20);
	      reset_screen_bold(0x40); /* add by KUWA */
	      bold = 0;
	      goto normal_out;
	  }
	  tmp = char_len(c_b->buffer[k]);
	  if(col + tmp > c_b->vlen ){
	      reset_screen_reverse(0x10,0x20);
	      reset_screen_bold(0x40); /* add by KUWA */
	      bold = 0;
	      goto end_syori;
	  }
	  w_putchar_move(c_b->buffer[k]);
	  if(col > end_of_line - CHAR_BYTE){
	      flushw_buf();
	      if(col < end_of_line ){
		  reset_screen_reverse(0x10,0x20);
	          reset_screen_bold(0x40); /* add by KUWA */
	          bold = 0;
		  putchar_move(' ');
		  set_screen_reverse(0x10,0x20);
		  if(k >= bst && k < bend) {
		    set_screen_bold(0x40);
		    bold = 1;
		  }
	      }
	      throw(end_of_line);
	      end_of_line += maxlength;
	  }
      }
      reset_screen_reverse(0x10,0x20);
      reset_screen_bold(0x40); /* add by KUWA */
      bold = 0;
  }

 normal_out:
  if(clr_l == 1){
    clr_line();
    oldmaxcol = col;
  }else if(clr_l == 2){
    clr_line();
    oldmaxcol = col;
  }else{
      oldmaxcol = max(oldmaxcol , col);
  }
  flush();
  return;

 end_syori:
#ifndef	X11
    for(k = col ; k <= c_b->vlen ; k++){
	putchar_move('$');
  }
#endif	!X11
  oldmaxcol = col;
  flush();
  return;
}

#ifndef	X11
/* �Ԥ���Ƭ���� */
static
t_cont_line_note()
{
#ifndef	WNMO
  throw0(0);
  putchar_norm('$');
#else
	if (_disp_window != 0) {
		TTYM_note();
	} else {
		throw0(0);
		putchar_norm('$');
	}
#endif
}

/* �Ԥ���Ƭ���� */
t_cont_line_note_delete()
{
#ifndef	WNMO
  throw0(0);
  putchar_norm(' ');
#else
	if (_disp_window != 0) {
		TTYM_note_delete();
	} else {
		throw0(0);
		putchar_norm(' ');
	}
#endif
}
#endif	!X11

/*���̤Υ����0���Хåե�����start_point �λ��������c �ˤ���ʸ�����֤�*/
static 
find_character_on_that_col(c , start_point)
int c;     /* colum */
int start_point; /* in_buffer as vst */
{

  int k;
  register int end_of_line = maxlength - disp_mode_length - 1 - c_b->start_col ;
  int len = 0;
  for(k = start_point ;k <= c_b->maxlen ;k++ ){
    len += char_len(c_b->buffer[k]);
    if(len >= c) return(k);
    if(len  > end_of_line - CHAR_BYTE){
	len = end_of_line;
	end_of_line += maxlength;
    }
  }	
  /*error but default to 0 */
  return(0);
}

/*���̤Υ����0���Хåե�����start_point �λ���ʸ��cp�β��̾�ΰ��֤��֤���*/
/* static */
cur_ichi(cp , start_point)
register int cp;
int start_point;
{
    register int k;
    register int end_of_line = maxlength - disp_mode_length - 1 - c_b->start_col ;
    register int len = 0;
    if(cp < start_point)return(-1);

    for(k = start_point ;k < cp ;k++ ){
	len += char_len(c_b->buffer[k]);
	if(len  > end_of_line - CHAR_BYTE){
	    len = end_of_line;
	    end_of_line += maxlength;
	}
    }	
    return(len);
}


print_buf_msg(msg)
{
#ifndef	X11
  push_cursor();
#endif
  throw_c(0);
  printf(msg);
#ifndef	X11
  pop_cursor();
  flush();
#endif
}

/* vst�򥻥åȤ���*/
/* returns -1 if not changed
   else returns new start colum
*/
#ifndef	X11
static int
set_vst()
{
  int tmp;
  int vst1;

  tmp = cur_ichi(c_b->t_c_p , 0);
  vst1 = find_character_on_that_col((tmp / (c_b->vlen - c_b->duplicate))*(c_b->vlen - c_b->duplicate), 0);
  if(cur_ichi(c_b->t_m_start , vst1) >= c_b->vlen){
      vst1 = c_b->t_c_p;
  }
  c_b->vst = vst1;
  return(vst1);
}
#endif	!X11

static char rk_modes[80];
extern char *romkan_dispmode(), /* added by Nide, 8/21 */ *romkan_offmode();

disp_mode()
{
  char	*p;
  strcpy(rk_modes, (NULL == (p = romkan_dispmode()) ? "[   ]" : p));
#ifndef	X11
  tty_disp_mode(rk_modes);
#else	X11
  jw_disp_mode(rk_modes);
#endif	!X11
}

/* cursor status is saved before call it */
display_henkan_off_mode()
{
  char	*p;

  strcpy(rk_modes, (NULL == (p = romkan_offmode()) ? "[---]" : p));
#ifndef	X11
  tty_disp_mode(rk_modes);
#else	X11
  jw_disp_mode(rk_modes);
#endif	!X11
}

/* ������������Ф�*/
t_throw()
{
    throw0(col + 1);
}

clr_line()
{
    clr_end_screen();
}	    
