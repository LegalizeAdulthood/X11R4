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
 *	Original Source: ./lib/Wnn/romkan/rk_extvars.h
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
/**********************************************************************
			rk_mult.h
***********************************************************************/

#ifndef	ROMKAN_LIB
#define	ROMKAN_LIB

typedef struct	_dat /* ���ϡ����ϡ��Хåե��Ĥ����ؤΥݥ��� */
{
	unsigned int	*code[3];
} dat;

typedef struct	_modestat {
	unsigned char	moderng; /* �⡼�ɤξ��֤κ����͡ܣ� */
	unsigned char	curmode; /* �⡼�ɤξ��� */
} modestat;

typedef struct	_hyo
 /* ��Ĥ��б�ɽ�Ρ��Ѵ��ǡ����ڤӳ��ѿ����Ѱ�Ρ���Ǽ���ؤΥݥ��󥿡�*/
{
	dat	*data;		/* �Ѵ��ǡ����γ������� */
	unsigned int		**hensudef;	/* �Ѱ�ɽ�γ������� */
} hyo; /* ɽ���Ȥ��Ѱ衦�б��ǡ����γ������� */

typedef struct	_matchpair /*���ߥޥå�����Ƥ����ѿ����ֹ�ȡ��ޥå�����ʸ����
				�Ȥ��ݻ���*/
{
	int	hennum;
	unsigned int	ltrmch;
} matchpair;

typedef struct _Romkan {
char	rk_errstat; /* (error)�ˤ�äƥ��顼�������������줿��1��â��
			   (eofflg)��Ω�äƤ������������ޤǤζ������Ͻ�����
			   �����ä�����2�ˤˤʤ롣romkan_henkan()�¹Ԥ��Ȥ�
			   ��������롣����ϥ桼���ˤⳫ����*/

int	flags;
 /* �ʲ��Υե饰��OR��
	RK_CHMOUT �⡼�ɥ����󥸤��Τ餻�뤫�Υե饰 
	RK_KEYACK ����������Ф�ɬ�������֤����Υե饰 
	RK_DSPNIL romkan_disp(off)mode�����ͤΥǥե���Ȥ���ʸ����Ǥ��뤫��
		  �ե饰��Ω���ʤ���Хǥե���Ȥ�NULL���ߴ����Τ���romkan_
		  init2�ޤǤϤ��줬Ω�ġ�
	RK_NONISE �������ɤ�Ф��ʤ��褦�ˤ��뤫�Υե饰
	RK_REDRAW Wnn�ǻȤ��ü�ʥե饰�����줬Ω�äƤ����硢romkan_henkan
		  �η�̤Ȥ���disout��nisedl�ʵ�ʪ��del�˰ʸ夬�ü쥳���ɤΤ�
		  �ǽ����ʸ�����֤äƤ����顢���θ���REDRAW��Ĥʤ���Wnn
		  ���Ѵ��Ԥ�redraw�򤹤뤿���ɬ�פ����֡�
	RK_SIMPLD delete��ư���ñ��ˤ��뤫�Υե饰
	RK_VERBOS verbose�ǵ��������Υե饰
 */

jmp_buf env0;

FILE	*modefile; /* �⡼�����ɽ�Υե����� */
char	nulstr[1];

char	*hcurread, *mcurread; /* �Ѵ�ɽ���⡼��ɽ�θ��߹�buf�ؤΥݥ��� */
char	*curfnm, *curdir; /* �����ɤ�Ǥ��Ѵ�ɽ���⡼��ɽ��̾�ȥǥ��쥯�ȥꡣ
     â��curdir�ϡ��������ѥ����ܤ��Ƥ���ɽ�Υ����ץ�������������Τ�ͭ�� */
unsigned int	*ltrbufbgn; /* �������letter�ΥХåե�����Ƭ��ؤ� */
  /* �����ޤĤϡ�readdata()�ʤɤδؿ���ǥ��������������������Ƭ���
     �������äơ����δؿ���ȴ������Ǥ������ͤ�Ȥ�ʤ��褦��ʬ��ա��äˡ�
     ���顼�ٹ�롼����ʤɤǤϡ�longjmp����������ˤ�����print��Ԥ�����
     �ʥ��顼�����Ѥ˻ȤäƤ���Τϡ��夫��͸Ĥޤǡˡ�*/
FILE	*nestfile[FILNST], **base; /* �Ѵ��б�ɽ�Υե����� */

modestat  modesw[MODMAX];

char	hyoshu[HYOMAX]; /* ɽ�μ��̡������ܡ������ɽ�ζ��̡ˤ���������� */
char	**modmeiptr, *modmeibgn[MODMAX]; /* �⡼��̾���Ф�����Ʊ�ͤΥǡ���*/
char	*modmeimem, modmeimem_[MODMEI]; /*		��		*/
char	**dspnamptr, *dspnambgn[DMDMAX];
				/* �⡼��ɽ��ʸ������Ф���Ʊ�ͤΥǡ��� */
char	*dspcod, dspcod_[DMDCHR]; /*		��		*/
char	*dspmod[2][2];
	 /* ���ߵڤӰ�����Ρ�romkan�򤽤줾��on��off���Ƥ�����Υ⡼��ɽ��
	    ʸ����ؤΥݥ��󥿡�romkan_dispmode()���֤��ͤ�dspmod[0][0]��*/

char	**hyomeiorg, **hyomeiptr;
	 /* ɽ��̾�ؤΥݥ��󥿤���������� *hyomeibgn[HYOMAX] �� readdata() ��
	    �Ȥ�����������Ƭ�ڤ���������ǤؤΥݥ��� */
char	*hyomeimem;
	 /* ɽ��̾�μºݤ�ʸ�������������� hyomeimem_[HYOMEI] �� readdata()
	    �ǻȤ�����������������ǤؤΥݥ��� */
char	**pathmeiorg, **pathmeiptr;
	 /* �б�ɽ�Υ������ѥ�̾�ؤΥݥ��󥿤���������� *pathmeibgn[PTHMAX]��
	    readdata() �ǻȤ�����������Ƭ�ڤ���������ǤؤΥݥ��� */
char	*pathareaorg, *pathmeimem;
	 /* �嵭�������ѥ�̾�μºݤ�ʸ�������������� pathmeimem_[PTHMEI] ��
	    readdata() �ǻȤ�����������Ƭ�ڤ���������ǤؤΥݥ��󥿡�
	    â��pathmeimem_����Ƭ�ˤϥ⡼��ɽ�Τ���ǥ��쥯�ȥ��̾�����ꡢ��
	    �ǻȤ��Τǡ�pathareaorg�ˤϤ���³���Υ��ꥢ����Ƭ�����Ϥ����롣*/

int	usemaehyo[HYOMAX], usehyo[HYOMAX], useatohyo[HYOMAX];
	 /* �����ܡ������ɽ�Τɤ줬���򤵤�Ƥ��뤫�Υǡ��� */
int	naibu_[NAIBMX], *naibu; /* �⡼��ɽ������ɽ������������� */

unsigned int	*lptr; /* letter�ؤ����ѥݥ��� */

unsigned int	rk_input; /* ���ϡ�3�Х��ȤޤǤ��ѹ��ʤ����б��� */
unsigned int	disout[OUTSIZ]; /* �ܽ�������ν��ϤΥХåե� */
unsigned int	rk_output[OUTSIZ]; /* ���������ν��ϡʺǽ����ϡˤΥХåե� */
unsigned int	keybuf[KBFSIZ], urabuf[KBFSIZ]; /* �ܽ����Хåե��Ȥ��������ΰ� */
int	lastoutlen, lastkbflen;
unsigned int	oneletter[2], nil[1];
int	hyonum;

hyo hyo_n[HYOMAX];

unsigned int	memory[SIZALL]; /* �Ѵ��б�ɽ�䡢�ѿ����Ѱ�ʤɤ�������ΰ� */
dat	data[LINALL]; /* �б�ɽ�ΰ�Ԥ��ȤΥǡ����ؤΥݥ��� */
unsigned int	*hensudefhyo[VARTOT]; /* �ѿ����Ȥ��Ѱ�ǡ����ؤΥݥ��� */
matchpair	henmatch[VARTOT]; /* �ѿ��Υޥå������򤿤�Ƥ��� */

#ifdef KDSP
#  ifdef MVUX
unsigned int	displine[DSPLIN]; /* �ǥХå���	 ɽ���ΰ��ʬ�򵭲� */
#  endif
#endif

unsigned int	*curdis;
int	codein_len;
unsigned int	*codeout, *remainkbf;
unsigned int	ungetc_buf; /* romkan_ungetc����ʸ�����򤷤Ƥ����Хåե� */
unsigned int	unnext_buf; /* romkan_unnext��		   ��		  */

unsigned int	(*keyin_method)(); /* ��������ؿ��Υ��ɥ쥹 */
int	(*bytcnt_method)(); /* �Х��ȥ�����ȴؿ��Υ��ɥ쥹 */
int	(*kbytcnt_method)(); /* �������ϲ���ѥХ��ȥ�����ȴؿ��Υ��ɥ쥹 */
char	prv_modfnm[REALFN]; /* �⡼��ɽ̾�����Ϥ��Υѥ�̾�ˤ���¸ */
	    /* ������꤬�����ø�! */

char	eofflg; /* romkan_next()����LTREOF���褿�Ȥ��ˡ�romkan_henkan()���1
		   �ˤ��롣������ͤ���0�ʤ��Ѵ��б�ɽ��(error)��̵�뤷�ơ�
		   �ܽ����Хåե����̤���ꥳ���ɤ⡢����Ū���Ѵ������롣����
		   LTREOF���褿�������̤ˤ�����Ϥ�Ф��褦�˻��ꤵ��Ƥ����
		   ���ν�����Ԥ���â�������ν����ϵޤ����館���Դ����ˡ�����
		   �������eofflg��2��*/

unsigned int	evalbuf[2][2][OUTSIZ];
char	ebf_sw;
unsigned int	saishu_out[OUTSIZ];
unsigned int	delchr, delchr2, nisedl; /* DEL�Υ���饯������Ĥޤǻ��Ƥ�*/
} Romkan;

extern Romkan	*cur_rk;
extern Romkan	*romkan_init3();
#endif	ROMKAN_LIB
