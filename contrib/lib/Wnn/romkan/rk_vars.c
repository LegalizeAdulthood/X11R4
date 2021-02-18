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
/***********************************************************************
			rk_vars.c
						88. 6.16  �� ��

	�ץ���������İʾ�Υե�����ˤޤ����äƻȤ��ѿ���
	�ޤȤ��������Ƥ��롣
***********************************************************************/

/*	Version 3.0
 */

#include "rk_header.h"

char	rk_errstat = 0; /* (error)�ˤ�äƥ��顼�������������줿��1��â��
			   (eofflg)��Ω�äƤ������������ޤǤζ������Ͻ�����
			   �����ä�����2�ˤˤʤ롣romkan_henkan()�¹Ԥ��Ȥ�
			   ��������롣����ϥ桼���ˤⳫ����*/

int	flags = 0;
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
char	nulstr[1] = {'\0'};

char	*hcurread, *mcurread; /* �Ѵ�ɽ���⡼��ɽ�θ��߹�buf�ؤΥݥ��� */
char	*curfnm, *curdir; /* �����ɤ�Ǥ��Ѵ�ɽ���⡼��ɽ��̾�ȥǥ��쥯�ȥꡣ
     â��curdir�ϡ��������ѥ����ܤ��Ƥ���ɽ�Υ����ץ�������������Τ�ͭ�� */
letter	*ltrbufbgn; /* �������letter�ΥХåե�����Ƭ��ؤ� */
  /* �����ޤĤϡ�readdata()�ʤɤδؿ���ǥ��������������������Ƭ���
     �������äơ����δؿ���ȴ������Ǥ������ͤ�Ȥ�ʤ��褦��ʬ��ա��äˡ�
     ���顼�ٹ�롼����ʤɤǤϡ�longjmp����������ˤ�����print��Ԥ�����
     �ʥ��顼�����Ѥ˻ȤäƤ���Τϡ��夫��͸Ĥޤǡˡ�*/
FILE	*nestfile[FILNST], **base; /* �Ѵ��б�ɽ�Υե����� */

struct	modestat {
	modetyp	moderng; /* �⡼�ɤξ��֤κ����͡ܣ� */
	modetyp	curmode; /* �⡼�ɤξ��� */
} modesw[MODMAX];

char	hyoshu[HYOMAX]; /* ɽ�μ��̡������ܡ������ɽ�ζ��̡ˤ���������� */
char	**modmeiptr, *modmeibgn[MODMAX]; /* �⡼��̾���Ф�����Ʊ�ͤΥǡ���*/
char	*modmeimem, modmeimem_[MODMEI]; /*		��		*/
char	**dspnamptr, *dspnambgn[DMDMAX];
				/* �⡼��ɽ��ʸ������Ф���Ʊ�ͤΥǡ��� */
char	*dspcod, dspcod_[DMDCHR]; /*		��		*/
char	*dspmod[2][2] = {NULL, NULL, NULL, NULL};
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

letter	*lptr; /* letter�ؤ����ѥݥ��� */

letter	rk_input; /* ���ϡ�3�Х��ȤޤǤ��ѹ��ʤ����б��� */
letter	disout[OUTSIZ]; /* �ܽ�������ν��ϤΥХåե� */
letter	rk_output[OUTSIZ]; /* ���������ν��ϡʺǽ����ϡˤΥХåե� */
letter	keybuf[KBFSIZ], urabuf[KBFSIZ]; /* �ܽ����Хåե��Ȥ��������ΰ� */
int	lastoutlen, lastkbflen;
letter	oneletter[2] = {EOLTTR, EOLTTR}, nil[1] = {EOLTTR};
int	hyonum;

struct	funstr /* �Ȥ߹��ߴؿ����������ݻ�����struct */
{
	char	*fnname;	/* �ؿ�̾ */
	char	appear;		/* ���ϥ������������ϥ����������Хåե��Ĥ���
				   �˽񤱤뤫�ɤ����Υե饰���ӥå� */
	char	argnum;		/* �����θĿ� */
	char	fntype;		/* ��̤Υ����� */
};

struct	dat /* ���ϡ����ϡ��Хåե��Ĥ����ؤΥݥ��� */
{
	letter	*code[3];
};

struct	hyo
 /* ��Ĥ��б�ɽ�Ρ��Ѵ��ǡ����ڤӳ��ѿ����Ѱ�Ρ���Ǽ���ؤΥݥ��󥿡�*/
{
	struct	dat	*data;		/* �Ѵ��ǡ����γ������� */
	letter		**hensudef;	/* �Ѱ�ɽ�γ������� */
};

#define bit3(x, y, z) ((x) | ((y) << 1) | ((z) << 2))

struct	funstr	func[] = /* �ؿ����ɲäˤϤ����� mchevl(), evlis() �򤤤��� */
{
/*			�����ο���-1�Ĥ�����̤���ѡ�
	  ̾		    �и�     ��	 �����ס�0:ʸ���ؿ� 1:ʸ����ؿ�
	  ��		     ��	     ��	 ��	 2:�ü�ؿ� 3:��ǽ 4:��� */
	"defvar",	bit3(1,0,0), -1, 4, /*	0 */
	"include",	bit3(1,0,0), -1, 4,
	"toupper",	bit3(1,1,1),  1, 0,
	"tolower",	bit3(1,1,1),  1, 0,
	"off",		bit3(0,1,0), -1, 2,
	"on",		bit3(0,1,0), -1, 2, /*	5 */
	"switch",	bit3(0,1,0), -1, 2,
	"toupdown",	bit3(1,1,1),  1, 0,
	"tozenalpha",	bit3(1,1,1),  1, 0,
	"tohira",	bit3(1,1,1),  1, 0,
	"tokata",	bit3(1,1,1),  1, 0, /* 10 */
	"tohankata",	bit3(1,1,1),  1, 1,
	"tozenhira",	bit3(1,1,1),  1, 0,
	"tozenkata",	bit3(1,1,1),  1, 0,
	"+",		bit3(1,1,1),  2, 0,
	"-",		bit3(1,1,1),  2, 0, /* 15 */
	"*",		bit3(1,1,1),  2, 0,
	"/",		bit3(1,1,1),  2, 0,
	"%",		bit3(1,1,1),  2, 0,
	"last=",	bit3(1,0,0),  1, 2,  /* ľ���Υޥå�������==������? */
	"if",		bit3(1,0,0), -1, 2, /* 20 */
	"unless",	bit3(1,0,0), -1, 2,
	"restart",	bit3(0,1,0),  0, 3,
	"delchr",	bit3(1,1,1),  0, 0,  /* delchr���֤����������ޥ�� */
	"alloff",	bit3(0,1,0),  0, 2,  /* ���⡼�ɤ򤤤äڤ��OFF */
	"allon",	bit3(0,1,0),  0, 2, /* 25 */
	"bitand",	bit3(1,1,1),  2, 0,
	"bitor",	bit3(1,1,1),  2, 0,
	"bitnot",	bit3(1,1,1),  1, 0,
	"!",		bit3(0,1,0),  0, 2,  /* ΢�Хåե�������Ū���ꥢ */
	"atEOF",	bit3(1,0,0),  0, 2, /* 30 */
	"todigit",	bit3(1,1,1),  2, 1,
	"dakuadd",	bit3(1,1,1),  1, 1,  /* �������ղ� */
	"handakuadd",	bit3(1,1,1),  1, 1,  /* Ⱦ�������ղ� */
	"value",	bit3(1,1,1),  1, 0,
	"error",	bit3(0,1,0),  0, 3, /* 35 */
	"defconst",	bit3(1,0,0), -1, 4,
	"setmode",	bit3(0,1,0), -1, 2,
	"mode+",	bit3(0,1,0), -1, 2,
	"mode-",	bit3(0,1,0), -1, 2,
	"mode=",	bit3(1,0,0), -1, 2, /* 40 */
	"mode!=",	bit3(1,0,0), -1, 2,
	"mode<",	bit3(1,0,0), -1, 2,
	"mode>",	bit3(1,0,0), -1, 2,
	"send",		bit3(0,1,0),  1, 1,  /* ��� */
	NULL
};
	/* last=��unless�� ���ϥ����ɤΰ��֤ˤ����֤�����ʸ����ؿ������ʤΤ�
			�ܽ���ɽ�ˤ����񤱤ʤ���*/
	/* +,-,*,/,bitand,bitor,bitnot�� 3�Х���ʬ�α黻��Ԥ���*/
	/* atEOF�� �ե����������Ǥ�����ư�����ꤹ�뤿��Τ�Ρ�â��������
		ư����Դ�����*/

struct	hyo	hyo_n[HYOMAX]; /* ɽ���Ȥ��Ѱ衦�б��ǡ����γ������� */
