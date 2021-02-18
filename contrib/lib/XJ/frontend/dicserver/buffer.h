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
 *	Original Source: ./lib/Wnn/uum/buffer.h
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

/* buffer��ɽ���Τ������ϡ�hanten�ˤ�äƷ��ꤵ��롣
ȿž���뤫���������֤ˤ��뤫�����Ω�ˡ�
�Хåե�������ǡ�ʸƬ����mark�ޤǡ�mark����cursor�ޤǡ�
cursor����buffer�κǸ�ޤǤξ��֤���Ω�����ꤹ�뤳�Ȥ��Ǥ��롣
	 when mark is set
        	hanten bit0,bit1:      --- mark
		bit2,bit3:        mark --- cursor
		bit4,bit5:      cursor --- 
	  when mark is not set(-1)
		bit4,bit5:      
    first one....hanten
    second one...under_line
*/

struct buf{
  wchar  *buffer;	/* ��Ͽ�ѥХåե� */
  char *buf_msg; /** ��̾�����Ϥ�����ΥХåե���������ɽ������ʸ���� */
  int start_col; /** ��̾���ϻ��β��̾�Υ����Υ������Ȱ��� */
  int t_c_p; /** ��̾�����ѥХåե�����ǤΥ���������� */
  int hanten; /** ��̾�����ѥХåե�����ȿžɽ�������Ƥ��뤫�ɤ��� */
  int maxlen; /**��̾�����ѥХåե�������ˤϤ��äƤ���ʸ���� */
  int buflen; /** length of the buffer */
  int t_m_start;  	/* -1 : mark�����ꤵ��Ƥ��ʤ���*/
			/* 0 �ʾ� : �ޡ����ΰ���*/
  int t_b_st;	/**�ܡ����ɽ���γ��ϰ��� */ /* add by KUWA */
  int t_b_end;	/**�ܡ����ɽ���ν�λ���� */ /* add by KUWA */
  int vlen;    /*�Хåե�����ǥ����꡼���ɽ���������*/
  int duplicate;/* �����꡼�󤬥������뤹����ˡ���ʣ����ɽ������ʸ����*/

  int vst;    /* buffer����ǲ��̤�ɽ������Ƥ���ǽ�ΰ���*/
		/*����ϡ�����˾�Υ롼����Ǥ�����ʤ�����˾�ޤ�����*/
  int (*key_in_fun)(); /*�����˴ؿ������ꤵ��Ƥ���ȡ�key_table�˥Х���ɤ�
  �ʤ�ʸ�������Ϥ�
  ���������˥Хåե����ˤ���ʸ�������줺������ˤ��δؿ����ƤФ��*/
  int (*redraw_fun)();/* romkan kara redraw ga kaette kitatoki */
  int (*ctrl_code_fun)();/*�����˴ؿ������ꤵ��Ƥ���ȡ�key_table�˥Х���ɤ�
  �ʤ�����ȥ���ʸ�������Ϥ���������˥٥���Ĥ餹�����
  ���δؿ����ƤФ��*/
  int (**key_table)();	/*�����Х���ɤΥơ��֥�*/
  char *rk_clear_tbl;	/* romakn_clear �� hituyouka */
};

extern int *bunsetsu;
extern int *bunsetsuend;
extern int *touroku_bnst;
/* extern int bunsetsucnt; */
extern int touroku_bnst_cnt;

extern struct buf *c_b; /* �����Ѥ����Ƥ���buf�򼨤������Х��ѿ�*/
extern w_char *kill_buffer; /* ����Хåե����礭���ϡ�maxchg�ȤʤäƤ���*/
extern int kill_buffer_offset;/* ����Хåե����Ȥ��Ƥ���Ĺ��*/
/* ADD KURI */
extern w_char *remember_buf; /* ����Х��Хåե��礭���ϡ�maxchg�ȤʤäƤ���*/
